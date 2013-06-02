#include "env_memory.h"
 
/* The main registers of the AVM */
avm_memcell ax, bx, cx;
avm_memcell retval;

/* The stack pointers */
// Be careful here
unsigned int top= AVM_STACKSIZE-1;
unsigned int topsp;

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
 
avm_memcell * avm_translate_operand(vmarg_s * arg,avm_memcell * reg){
	switch(arg->type){
		case global_a: return &stack[arg->value];
		case local_a:  return &stack[topsp-arg->value];
		case formal_a: return &stack[topsp+AVM_STACK_ENV_SIZE+1+arg->value];
		case retval_a: return &retval;
		case integer_a: {
			reg->type = integer_m;
			reg->data.int_value = consts_getint(arg->value);
			return reg;
		}
		case double_a: {
			reg->type = double_m;
			reg->data.double_value = consts_getdouble(arg->value);
			return reg;
		}
		case string_a: {
			reg->type = string_m;
			reg->data.str_value = consts_getstr(arg->value);
			return reg;
		}
		case bool_a: {
			reg->type = bool_m;
			reg->data.bool_value = arg->value;
			return reg;
		}
		case nil_a: {
			reg->type = nil_m;
			return reg;
		}
		case userfunc_a: {
			reg->type = userfunc_m;
			reg->data.func_value = arg->value;
			return reg;
		}
		case libfunc_a: {
			reg->type = libfunc_m;
			reg->data.lib_func_value = libfuncs_getused(arg->value);
			return reg;
		}
		default: assert(0);
	}
}

void avm_warning(char * msg1,char * msg2,char * msg3,unsigned int line){
	fprintf(stdout,"\n\x1b[33mWarning : %s %s %s at line %d.\e[0m\n",msg1,msg2,msg3,line);
}

void avm_error(char * msg1,char * msg2,char * msg3, unsigned int line){
	fprintf(stdout,"\n\x1b[31mRuntime error : %s %s %s at line %d. \e[0m \n",msg1,msg2,msg3,line);
	fprintf(stdout,"\nThe program has exited with return code (0: BAD CODE).\n");
	exit(0);
}

void avm_anonymous_error(char * msg){
	fprintf(stdout,"\n\x1b[31mRuntime error : %s\e[0m\n",msg);
	fprintf(stdout,"\nThe program has exited with return code (0: BAD CODE).\n");
	exit(0);
}