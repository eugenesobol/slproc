%{

extern int yylineno;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sl_common.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;

void yyerror(const char* s);
extern void yyrestart(FILE *f);

int registers[4] = { 0 };

int register_get(char symbol);
void register_update(char symbol, int val);
void register_add(char sumbol, int val1, int val2); 
void register_sub(char symbol, int val1, int val2);

void jump(int line, int val1, int val2);

long sl_find_line(FILE *f, int lineno);
void sl_display_registers(void);

int verbose = 0;

%}
%union {int num; char reg;}
%start line

%token <num> number
%token <reg> registername

%token command_yaz
%token command_ekle
%token command_cikar
%token command_git

%%
line    :   command 
        |   line command
        |   error
        ;

command     :   cmd_yaz 
            |   cmd_ekle
            |   cmd_cikar
            |   cmd_git
            ;

cmd_yaz     :   command_yaz registername number     { register_update($2, $3); }
            |   command_yaz registername registername { register_update($2, register_get($3)); }
            ;

cmd_ekle    :   command_ekle registername number number  { register_add($2, $3, $4); }
            |   command_ekle registername registername number { register_add($2, register_get($3), $4); }
            |   command_ekle registername number registername { register_add($2, $3, register_get($4)); }
            |   command_ekle registername registername registername { register_add($2, register_get($3), register_get($4)); }
            ;

cmd_cikar   :   command_cikar registername number number { register_sub($2, $3, $4); }
            |   command_cikar registername registername number { register_sub($2, register_get($3), $4); }
            |   command_cikar registername number registername { register_sub($2, $3, register_get($4)); }
            |   command_cikar registername registername registername { register_sub($2, register_get($3), register_get($4)); }
            ;

cmd_git     :   command_git number number number { jump($2, $3, $4); }
            |   command_git number registername number { jump($2, register_get($3), $4); } 
            |   command_git number number registername { jump($2, $3, register_get($4)); } 
            |   command_git number registername registername { jump($2, register_get($3), register_get($4)); }
            ;

%%                              /* C code */

int main(int argc, char **argv) {

    if (argc <= 1) {
        yyin = stdin;
    }
    else {
        yyin = fopen(argv[1], "r");
        if (yyin == NULL) {
            fprintf(stderr, "No such file or directory\n");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-v", 2) == 0) {
            verbose = 1;
        }
    }

	do {
		yyparse();
	} while(!feof(yyin));

    if (yyin != stdin) {
        fclose(yyin);
        yyin = stdin;
    }

	return 0;
}

void yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s\n", s);
}

int register_get(char symbol)
{
    if (verbose)
        printf("register_get: %c\n", symbol);

    return registers[symbol - 'A'];
}

void register_update(char symbol, int val)
{
    if (verbose)
        printf("register_update: %c <- %d line: %d\n", symbol, val, yylineno);

    registers[symbol - 'A'] = val;
}

void register_sub(char symbol, int val1, int val2)
{
    if (verbose)
        printf("register_sub: %c <- %d - %d\n", symbol, val1, val2);

    register_update(symbol, val1 - val2);
}

void register_add(char symbol, int val1, int val2)
{
    if (verbose)
        printf("register_add %c <- %d + %d\n", symbol, val1, val2);

    register_update(symbol, val1 + val2);
}

void jump(int line, int val1, int val2)
{
    if (verbose)
        printf("jump to line: %d with values %d, %d\n", line, val1, val2);

    if (val1 == val2) {
        int rc;
        long pos = sl_find_line(yyin, line);
        if (pos < 0) {
            printf("Execution error: jump line doesn't exist\n");
            return;
        }

        yyrestart(yyin);

        rc = fseek(yyin, pos, SEEK_SET);
        if (rc < 0) {
            fprintf(stderr, "Execution error: seek position failed\n");
        }

        if (verbose)
            printf("seeking to position %ld result: %d\n", pos, rc);
    }
}

void sl_display_registers()
{
    int count = sizeof(registers) / sizeof(registers[0]);

    for(int i = 0; i < count; i++) {
        if (i < count - 1)
            printf("%d ", registers[i]);
        else
            printf("%d\n", registers[i]);

    }
}
