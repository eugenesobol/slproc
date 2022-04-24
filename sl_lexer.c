#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sl_lexer.h"
#include "sl_common.h"

extern int yylex();
extern void yyrestart();
extern void yyset_lineno(int);

extern int yylineno;
extern char yytext[];
extern FILE *yyin;
int sl_verbose = 0;

#if defined(SL_DEBUG_PARSED_COMMAND)

char sl_test_symbol(FILE *f, fpos_t spacing)
{
    char c;
    printf("Spaceing get: %lld", spacing);

    fseek(f, spacing, SEEK_SET);
    fread((void*)&c, 1, 1, f);
    return c;
}

#endif

void sl_waste_line()
{
    int ntoken;
    do {
        ntoken = yylex();
    } while(ntoken != SL_NEWLINE && ntoken != 0);
}

#define SL_TOKEN_CHECK(arg) if (sl_verbose) printf("<%s>\n", sl_token_to_string(arg)); if (!arg) break;

#define SL_CMD_DIMENSION 5
#define SL_CMD_SEQNS 1

int sl_command_yaz_sequence[SL_CMD_SEQNS][SL_CMD_DIMENSION] = {
    { SL_REGISTER, SL_REG_OR_INT, SL_ENDING, SL_NEWLINE, SL_NEWLINE }
};

int sl_command_ekle_sequence[SL_CMD_SEQNS][SL_CMD_DIMENSION] = {
    { SL_REGISTER, SL_REG_OR_INT, SL_REG_OR_INT, SL_ENDING, SL_NEWLINE }
};

int sl_command_cikar_sequence[SL_CMD_SEQNS][SL_CMD_DIMENSION] = {
    { SL_REGISTER, SL_REG_OR_INT, SL_REG_OR_INT, SL_ENDING, SL_NEWLINE }
};

int sl_command_git_sequence[SL_CMD_SEQNS][SL_CMD_DIMENSION] = {
    { SL_INTEGER, SL_REG_OR_INT, SL_REG_OR_INT, SL_ENDING, SL_NEWLINE }
};

struct SL_type {
    int type;
    union {
        int num;
        int reg;
    } value;
};

struct SL_type sl_run_command_sequence[SL_CMD_DIMENSION];

int registers[4] = {0};

int sl_extract_value(struct SL_type *s)
{
    int value;

    if (sl_is_token_register(s->type)) {
        value = registers[sl_register_index(s->value.reg)];
    }
    else if (sl_is_token_number(s->type)) {
        value = s->value.num;
    }

    return value;
}

void sl_store_value(struct SL_type *s, int value)
{
    int idx = sl_register_index(s->value.reg);
    if (sl_verbose) printf("register_update %c <- %d\n", idx+'A', value);
    registers[idx] = value;
}

void sl_dump_registers(void)
{
    int count = sizeof(registers) / sizeof(registers[0]);

    for(int i = 0; i < count; i++) {
        if (i == count - 1) {
            printf("%d\n", registers[i]);

        } else {
            printf("%d ", registers[i]);
        }
    }
}

void sl_jump_to_line(int lineno)
{
    fpos_t pos = sl_find_line(yyin, lineno);
    if (pos < 0) {
        printf("Runtime exception: jump line <%d> doesn't exist\n", lineno);
        return;
    }

    if (sl_verbose) printf("Jumping to line: %d\n", lineno);

    yyrestart(yyin);
    fseek(yyin, pos, SEEK_SET);
    yyset_lineno(lineno+1);
}

void sl_execute_command(struct SL_type sq[SL_CMD_DIMENSION])
{
    switch(sq[0].type) {
        case SL_COMMAND_YAZ: {
            sl_store_value(&sq[1], sl_extract_value(&sq[2]));
        }
        break;
        case SL_COMMAND_EKLE: {
            sl_store_value(&sq[1], sl_extract_value(&sq[2]) +
                                   sl_extract_value(&sq[3]));
        }
        break;
        case SL_COMMAND_CIKAR: {
            sl_store_value(&sq[1], sl_extract_value(&sq[2]) -
                                   sl_extract_value(&sq[3]));
        }
        break;
        case SL_COMMAND_GIT: {
            if (sl_extract_value(&sq[2]) == sl_extract_value(&sq[3]))
                sl_jump_to_line(sl_extract_value(&sq[1]));
        };
        break;
        default:
            fprintf(stderr, "Unknown command: %d\n", sq[0].type);
        break;
    }
}

void sl_usage(const char *cmd)
{
    printf("Usage example: %s <filename>\n", cmd);
}

void sl_parse_input(void)
{
    int ntoken;

    do {

        ntoken = yylex();
        SL_TOKEN_CHECK(ntoken);

        /* handle empty line */
        if (ntoken == SL_NEWLINE) {
            continue;
        }

        /* handle comment without code */
        if (ntoken == SL_COMMENT) {
            sl_waste_line();
            continue;
        }

        /* handle command at the line begin */
        if (!sl_is_token_command(ntoken)) {
            printf("Syntax error in line %d, Expected a command but found: %d\n", yylineno - 1, ntoken);
            sl_waste_line();
            continue;
        }

        int (*sequence)[SL_CMD_SEQNS][SL_CMD_DIMENSION] = NULL;

        /* detect acceptable token sequence for a command */
        switch (ntoken)
        {
        case SL_COMMAND_YAZ:
            sequence = &sl_command_yaz_sequence;
            break;
        case SL_COMMAND_EKLE:
            sequence = &sl_command_ekle_sequence;
            break;
        case SL_COMMAND_CIKAR:
            sequence = &sl_command_cikar_sequence;
            break;
        case SL_COMMAND_GIT:
            sequence = &sl_command_git_sequence;
            break;
        default:
            break;
        }

        if (!sequence) {
            fprintf(stderr, "Unhandled command exception\n");
            exit(EXIT_FAILURE);
        }

        int atoken;
        int parse_ok = 0;

        sl_run_command_sequence[0].type = ntoken;

        /* Check availible acceptable sequences desribed in sl_command_*_sequences array */
        for(int tr = 0; tr < SL_CMD_SEQNS; tr++) {
            for(int seq = 0; seq < SL_CMD_DIMENSION; seq++) {

                atoken = yylex();
                SL_TOKEN_CHECK(atoken);

                if (!sl_is_acceptable(atoken, (*sequence)[tr][seq])) {
                    printf("Syntax error at line: %d <%s> Expected: <%s>\n", yylineno - 1, sl_token_to_string(atoken), sl_token_to_string((*sequence)[tr][seq]));

                    if (atoken != SL_NEWLINE)
                        sl_waste_line();

                    break;
                }

                if (sl_is_token_number(atoken)) {
                    sl_run_command_sequence[seq + 1].value.num = atoi(yytext);
                    sl_run_command_sequence[seq + 1].type = atoken;
                } else if (sl_is_token_register(atoken)) {
                    sl_run_command_sequence[seq + 1].value.reg = atoken;
                    sl_run_command_sequence[seq + 1].type = atoken;
                } else if (atoken == SL_NEWLINE) {
                    break;
                }

                parse_ok = 1;
            }

            if (parse_ok)
                break;
        }

        if (parse_ok) {

#if defined(SL_DEBUG_PARSED_COMMAND)
            for(int i = 0 ; i < SL_CMD_DIMENSION; i++) {
                printf("[%d, %d, %d] ", sl_run_command_sequence[i].type,
                                                        sl_run_command_sequence[i].value.num,
                                                        sl_run_command_sequence[i].value.reg);
            }

            printf("\n");
#endif
            sl_execute_command(sl_run_command_sequence);
        }

    } while(1);

    sl_dump_registers();

}

int main(int argc, char **argv)
{
    int argi;
    char *filename = NULL;

    /*
     * Basic options handling
     * It's better to use some of opt's library
     */
    if (argc < 2) {
        sl_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    for(argi = 1; argi < argc; argi++) {
        if (strncasecmp(argv[argi], "-v", 2) == 0) {
            sl_verbose = 1;
        }
        else {
            filename = argv[argi];
        }
    }

    if (!filename) {
        sl_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    yyin = fopen(filename, "r");
    if (yyin == NULL) {
        fprintf(stderr, "No such file or direcroty\n");
        exit(EXIT_FAILURE);
    }

    sl_parse_input();

    return EXIT_SUCCESS;
}
