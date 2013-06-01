#include "file_handler.h"
#include "env_memory.h"
#include "instructions.h"

int main(int argc, char * argv[]){
    if (argc <= 1)
    	error_message("No filename was given as an argument.");
    read_binary_file(argv[1]);

    fprintf(stdout,"The executable binary file (%s) has been loaded.\n",argv[1]);

	return 0;
}