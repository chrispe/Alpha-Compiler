al: lex.yy.c
	@echo "Generating the executable file..."
	@gcc -o al lex.yy.c -lfl
	@echo "Done."
lex.yy.c: lex_analyzer.l
	@echo "Generating lex.yy.c..."
	@flex lex_analyzer.l
clean:
	@echo "Cleaning up..."
	@rm -f al
	@rm -f lex.yy.c
	@echo "Done."
