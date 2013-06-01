avm: file_handler.o mem_manager.o env_mem.o assign.o avm.c 
	gcc -o avm file_handler.o mem_manager.o env_mem.o assign.o avm.c
	rm *.o
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
