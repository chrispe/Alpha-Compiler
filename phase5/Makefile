avm: file_handler.o mem_manager.o env_mem.o assign.o table.o call.o arithm.o jumps.o dispatcher.o lib.o avm.c 
	gcc -o avm file_handler.o mem_manager.o env_mem.o assign.o dispatcher.o call.o jumps.o arithm.o lib.o avm.c
	rm *.o	
lib.o: library_funcs.c library_funcs.h
	gcc -o lib.o -c library_funcs.c
dispatcher.o: dispatcher.c dispatcher.h	
	gcc -o dispatcher.o -c dispatcher.c
jumps.o: ./instructions/jumps.c ./instructions/jumps.h
	gcc -o jumps.o -c ./instructions/jumps.c	
arithm.o: ./instructions/arithmetic.c ./instructions/arithmetic.h
	gcc -o arithm.o -c ./instructions/arithmetic.c	
call.o: ./instructions/call.c ./instructions/call.h
	gcc -o call.o -c ./instructions/call.c	
table.o: ./instructions/table.c ./instructions/table.h
	gcc -o table.o -c ./instructions/table.c
assign.o: ./instructions/assign.c ./instructions/assign.h 
	gcc -o assign.o -c ./instructions/assign.c
env_mem.o: env_memory.c env_memory.h
	gcc -o env_mem.o -c env_memory.c	
mem_manager.o: memory_manager.h memory_manager.c
	gcc -o mem_manager.o -c memory_manager.c
file_handler.o: file_handler.c file_handler.h
	gcc -o file_handler.o -c file_handler.c
clean:
	rm -f *.o
	rm -f avm
