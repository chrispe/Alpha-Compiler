 
#include "env_memory.h"

int main(int argc, char * argv[]){
    if (argc <= 1)
    	error_message("No filename was given as an argument.");
    read_binary_file(argv[1]);

    fprintf(stdout,"The executable binary file (%s) has been loaded.\n",argv[1]);
	userfunc_s * s = userfuncs_getfunc(1);
	printf("The func : %s\n",s->name);
	return 0;
}