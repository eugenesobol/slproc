%{
#include "sl_lexer.h"

%}

%%
"YAZ"       return SL_COMMAND_YAZ;
"EKLE"      return SL_COMMAND_EKLE;
"CIKAR"     return SL_COMMAND_CIKAR;
"GIT"       return SL_COMMAND_GIT;

"REGA"      return SL_REGISTER_A;
"REGB"      return SL_REGISTER_B;
"REGC"      return SL_REGISTER_C;
"REGD"      return SL_REGISTER_D;

[1-9][0-9]*      return SL_INTEGER;
[ \t]        {;}
\n          return SL_NEWLINE;
[#].*       return SL_COMMENT;
.           {}

%%

char *sl_token_to_string(int token)
{
    switch(token) {
        case SL_COMMAND_YAZ:
            return "YAZ";
        case SL_COMMAND_EKLE:
            return "EKLE";
        case SL_COMMAND_CIKAR:
            return "CIKAR";
        case SL_COMMAND_GIT:
            return "GIT";
        case SL_REGISTER_A:
            return "REGA";
        case SL_REGISTER_B:
            return "REGB";
        case SL_REGISTER_C:
            return "REGC";
        case SL_REGISTER_D:
            return "REGD";
        case SL_INTEGER:
            return "NUMBER";
        case SL_COMMENT:
            return "COMMENT";
        case SL_NEWLINE:
            return "\\n";
        case SL_COMMAND:
            return "COMMAND";
        case SL_REGISTER:
            return "REGISTER";
        case SL_REG_OR_INT:
            return "REGISTER OR INTEGER";
        case SL_ENDING:
            return "LINE ENDING";
        default:
            return "UNKNOWN";
    }
}

int yywrap(void)
{
    return 1;
}
