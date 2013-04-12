parser: lex.yy.c parser.c lex_lib.o st.o
	gcc -o parser lex.yy.c lex_lib.o st.o parser.c -lfl
	rm *.o
	rm -f lex.yy.c
	rm -f parser.c
	rm -f parser.h
parser.c:
	bison --yacc parser.y
lex.yy.c:
	flex lex_analyzer.l
lex_lib.o: lex_lib.c lex_lib.h
	gcc -o lex_lib.o -c lex_lib.c
st.o: symbol_table.c symbol_table.h
	gcc -o st.o -c symbol_table.c
clean:
	rm -f *.o
	rm -f lex.yy.c
	rm -f parser.c
	rm -f parser.h
	rm -f parser
