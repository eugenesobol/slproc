%{
#include "y.tab.h"
extern void yyerror(const char *);
extern void sl_display_registers(void);
%}

%%

"YAZ"       return command_yaz;
"EKLE"      return command_ekle;
"CIKAR"     return command_cikar;
"GIT"       return command_git;

"REGA"|"REGB"|"REGC"|"REGD"      { yylval.reg = yytext[3]; return registername; }
[0-9]+                           { yylval.num = atoi(yytext); return number; }
"#".*       {}
[ \n\t]     ;
.           ;

%%
int yywrap(void)
{
    sl_display_registers();
    return 1;
}