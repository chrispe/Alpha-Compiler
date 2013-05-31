avm: file_handler.o avm.c
	gcc -o avm file_handler.o avm.c
	rm *.o
file_handler.o: file_handler.c file_handler.h
	gcc -o file_handler.o -c file_handler.c
clean:
	rm -f *.o
	rm -f avm
