
all: slproc

y.tab.c: sl_scanner.y
	@echo "[YACC] $<"
	@yacc -d $^

sl_scanner.yy.c: sl_scanner.l y.tab.c y.tab.h
	@echo "[LEX] $<"
	@lex -o $@ -l $<

slproc: sl_scanner.yy.c y.tab.c
	@echo "[GCC] $^"
	@gcc $^ -o $@
	@echo "[LINK] $@"

# Another one target to try different approach of using lex without yacc
# 
#snproc_lex: sl_lexer.yy.c sl_lexer.c
#	@echo "[GCC] $^"
#	@gcc $^ -g -o $@
#	@echo "[LINK] $@"

#sl_lexer.yy.c: sl_lexer.l
#	@echo "[LEX] $<"
#	@lex -o $@ -l $<

clean:
	@rm -f y.tab.h y.tab.c sl_lexer.yy.c sl_scanner.yy.c slproc slproc_lex

.PHONY: sl_scanner.c sl_scanner.l sc_scanner.h sl_scanner.y y.tab.c sl_lexer.yy.c
