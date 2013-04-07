parser: lex.yy.c parser.c lex_lib.o st.o
	@echo "Generating executable file..."
	@gcc -o parser lex.yy.c lex_lib.o st.o parser.c -lfl
	@echo "Cleaning up..."
	@rm *.o
	@rm -f lex.yy.c
	@rm -f parser.c
	@rm -f parser.h
	@echo "Done."
parser.c:
	@echo "Generating parser.c using Bison..."
	@bison --yacc parser.y
lex.yy.c:
	@echo "Generating lex.yy.c using Flex..."
	@flex lex_analyzer.l
lex_lib.o: lex_lib.c lex_lib.h
	@gcc -o lex_lib.o -c lex_lib.c
st.o: symbol_table.c symbol_table.h
	@gcc -o st.o -c symbol_table.c
clean:
	@echo "Cleaning up..."
	@rm -f *.o
	@rm -f lex.yy.c
	@rm -f parser.c
	@rm -f parser.h
	@rm -f parser
	@echo "Done."
