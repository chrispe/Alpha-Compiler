#include "file_handler.h"
#include "env_memory.h"
#include "instructions.h"
#include "dispatcher.h"

int main(int argc, char * argv[]){
    if (argc <= 1)
    	error_message("No filename was given as an argument.");

    char debug_mode = 0;

    if(argc>=3){
        if(strcmp(argv[2],"-d")==0)
            debug_mode = 1;
    }

    read_binary_file(argv[1]);
    fprintf(stdout,"The executable binary file (%s) has been loaded.\n",argv[1]);

    avm_init_stack();
    fprintf(stdout, "The stack has been initialized.\n");

    fprintf(stdout,"Press [ENTER] to begin the execution.\n");
    getchar();
    
    //topsp = top;
    while(!execution_finished){
        if(debug_mode)
            printstack();
    	execute_cycle();
    }
      
    fprintf(stdout,"\nThe program has exited with return code (1: OK).\n");
	return 0;
}