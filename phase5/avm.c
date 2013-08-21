#include "file_handler.h"
#include "env_memory.h"
#include "instructions.h"
#include "dispatcher.h"

int main(int argc, char * argv[]){

    unsigned char debug_mode = 0;

    if (argc <= 1)
    	error_message("Missing filename argument. \n\tAdd the path to the executable file as a parameter.");

    if(argc>=3){
        if(strcmp(argv[2],"-d")==0)
            debug_mode = 1;
    }

    read_binary_file(argv[1]);
    if(debug_mode)
        fprintf(stdout,"The executable binary file (%s) has been loaded.\n",argv[1]);

    avm_init_stack();
    if(debug_mode)
        fprintf(stdout, "The stack has been initialized.\n");

    if(debug_mode){
        fprintf(stdout,"Press [ENTER] to begin the execution.\n");
        getchar();
    }

    while(!execution_finished){
        if(debug_mode)
            printstack();
    	execute_cycle();
    }
     
    if(debug_mode)    
        fprintf(stdout,"\nThe program has exited with return code (1: OK).\n");
    
	return 0;
}