#ifndef SL_LEXER_H
#define SL_LEXER_H

#define SL_COMMAND_YAZ (1 << 0)
#define SL_COMMAND_EKLE (1 << 1)
#define SL_COMMAND_CIKAR (1 << 2)
#define SL_COMMAND_GIT (1 << 3)
#define SL_COMMAND (SL_COMMAND_YAZ | SL_COMMAND_CIKAR | SL_COMMAND_EKLE | SL_COMMAND_GIT)

#define SL_REGISTER_A (1 << 4)
#define SL_REGISTER_B (1 << 5)
#define SL_REGISTER_C (1 << 6)
#define SL_REGISTER_D (1 << 7)
#define SL_REGISTER (SL_REGISTER_A | SL_REGISTER_B | SL_REGISTER_C | SL_REGISTER_D)

#define SL_INTEGER (1 << 8)
#define SL_REG_OR_INT (SL_REGISTER | SL_INTEGER)

#define SL_COMMENT (1 << 9)
#define SL_NEWLINE (1 << 10)
#define SL_ENDING (SL_COMMENT | SL_NEWLINE)

extern char *sl_token_to_string(int token);

static inline int sl_is_token_command(int ntoken)
{
    return ntoken & SL_COMMAND;
}

static inline int sl_is_token_register(int ntoken)
{
    return ntoken & SL_REGISTER;
}

static inline int sl_is_token_number(int ntoken)
{
    return ntoken & SL_INTEGER;
}

static inline int sl_is_acceptable(int ntoken, int exp)
{
    return ntoken & exp;
}

static inline int sl_register_index(int reg) {
    switch(reg) {
        case SL_REGISTER_A:
            return 0;
        case SL_REGISTER_B:
            return 1;
        case SL_REGISTER_C:
            return 2;
        case SL_REGISTER_D:
            return 3;
    }
    return -1;
}

#endif //SL_LEXER_H
