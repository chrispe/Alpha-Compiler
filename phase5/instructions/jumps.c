#include "jumps.h"

tobool_func_t to_bool_funcs[] = {
	double_tobool,
	int_tobool,
	string_tobool,
	bool_tobool,
	table_tobool,
	userfunc_tobool,
	libfunc_tobool,
	nil_tobool,
	undef_tobool
};

unsigned char double_tobool(avm_memcell * m){
	return m->data.double_value!=0;
}

unsigned char int_tobool(avm_memcell * m){
	return m->data.int_value!=0;
}

unsigned char string_tobool(avm_memcell * m){
	return m->data.str_value[0]!=0;
}

unsigned char bool_tobool(avm_memcell * m){
	return m->data.bool_value;
}

unsigned char table_tobool(avm_memcell * m){
	return 1;
}

unsigned char userfunc_tobool(avm_memcell * m){
	return 1;
}

unsigned char libfunc_tobool(avm_memcell * m){
	return 1;
}

unsigned char nil_tobool(avm_memcell * m){
	return 0;
}

unsigned char undef_tobool(avm_memcell * m){
	return 0;
}

unsigned char avm_tobool(avm_memcell * m){
	return (*to_bool_funcs[m->type])(m);
}

void execute_jump(instr_s * instr){
	pc = instr->result->value;
}