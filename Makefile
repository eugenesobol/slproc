
all: slproc

### lex implementation

sl_lexer.yy.c: sl_lexer.l
	@echo "[LEX] $<"
	@lex -o $@ -l $<

slproc: sl_lexer.yy.c sl_lexer.c
	@echo "[GCC] $^"
	@gcc $^ -o $@
	@echo "[LINK] $@"

### yacc implementation

y.tab.c: sl_scanner.y
	@echo "[YACC] $<"
	@yacc -d $^

sl_scanner.yy.c: sl_scanner.l y.tab.c y.tab.h
	@echo "[LEX] $<"
	@lex -o $@ -l $<

slproc_yacc: sl_scanner.yy.c y.tab.c
	@echo "[GCC] $^"
	@gcc $^ -o $@
	@echo "[LINK] $@"

clean:
	@rm -f y.tab.h y.tab.c sl_lexer.yy.c sl_scanner.yy.c slproc slproc_yacc

.PHONY: all clean
