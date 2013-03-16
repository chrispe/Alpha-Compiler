al: lex.yy.c token.o
	@echo "Generating executable file..."
	@gcc -o al lex.yy.c token.o -lfl
	@echo "Cleaning up..."
	@rm *.o
	@rm -f lex.yy.c
	@echo "Done."
token.o: token.c token.h
	@gcc -o token.o -c token.c
lex.yy.c: lex_analyzer.l
	@echo "Generating lex.yy.c using Flex..."
	@flex lex_analyzer.l
clean:
	@echo "Cleaning up..."
	@rm -f *.o
	@rm -f al
	@rm -f lex.yy.c
	@echo "Done."
