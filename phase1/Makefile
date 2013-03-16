al: lex.yy.c
	@echo "Generating the executable file..."
	@gcc -o al lex.yy.c -lfl
	@echo "Done."
lex.yy.c: scanner.l
	@echo "Generating lex.yy.c..."
	@flex scanner.l
clean:
	@echo "Cleaning up..."
	@rm -f al
	@rm -f lex.yy.c
	@echo "Done."
