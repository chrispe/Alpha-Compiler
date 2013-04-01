al: lex.yy.c token.o
	@echo "Generating executable file..."
	@gcc -o al lex.yy.c token.o -lfl
	@echo "Cleaning up..."
	@rm *.o
	@echo "Done."
token.o: token.c token.h
	@gcc -o token.o -c token.c
clean:
	@echo "Cleaning up..."
	@rm -f *.o
	@rm -f al
	@echo "Done."
