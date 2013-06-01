#include "env_memory.h"

/* The main registers of the AVM */
avm_memcell ax, bx, cx;
avm_memcell retval;

/* The stack pointers */
unsigned int top, topsp;

double consts_getdouble(unsigned int index){
	return(double_consts[index]);
}

int consts_getint(unsigned int index){
	return(integer_consts[index]);
}

char * libfuncs_getused(unsigned int index){
	return(named_lib_funcs[index]);
}

char * consts_getstr(unsigned int index){
	return(str_consts[index]);
}

userfunc_s * userfuncs_getfunc(unsigned int index){
	return(&user_funcs[index]);
}