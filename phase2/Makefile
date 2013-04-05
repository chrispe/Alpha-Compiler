calc: lex.yy.c parser.c lex_lib.o 
	@echo "Generating executable file..."
	@gcc -o calc lex.yy.c lex_lib.o parser.c -lfl
	@echo "Cleaning up..."
	@rm *.o
	@rm -f lex.yy.c
	@rm -f parser.c
	@rm -f parser.h
	@echo "Done."
parser.c:
	@echo "Generating parser.c using Bison..."
	@bison --yacc --defines parser.y
lex.yy.c:
	@echo "Generating lex.yy.c using Flex..."
	@flex lex_analyzer.l
lex_lib.o: lex_lib.c lex_lib.h
	@gcc -o lex_lib.o -c lex_lib.c
clean:
	@echo "Cleaning up..."
	@rm -f *.o
	@rm -f calc
	@echo "Done."
