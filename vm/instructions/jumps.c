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

cmp_func cmp_functions[] = {
	is_less_eq,
	is_greatereq,
	is_less,
	is_greater
};
 
unsigned char is_greater(double a, double b){
	return a > b;
}

unsigned char is_greatereq(double a, double b){
	return a>=b;
}

unsigned char is_less(double a, double b){
	return a<b;
}

unsigned char is_less_eq(double a, double b){
	return a<=b;
}
 
void execute_cmp(instr_s * instr){
	assert(instr->result->type == label_a);

	avm_memcell * rv1 = avm_translate_operand(instr->arg1,&ax);
	avm_memcell * rv2 = avm_translate_operand(instr->arg2,&bx);

	if(!is_num_type(rv1->type))
		avm_error("Cannot compare, variable (",instr->arg1->name,") has invalid type.",instr->line);
	if(!is_num_type(rv2->type))
		avm_error("Cannot compare, variable (",instr->arg2->name,") has invalid type.",instr->line);

	unsigned char result = 0;
	cmp_func func = cmp_functions[instr->opcode-jle_v];
 
	if(rv1->type == integer_m && rv2->type==integer_m)
		result = (*func)(rv1->data.int_value,rv2->data.int_value);
	else if(rv1->type == integer_m)
		result = (*func)((double)rv1->data.int_value,rv2->data.double_value);
	else if(rv2->type == integer_m)
		result = (*func)(rv1->data.double_value,(double)rv2->data.int_value);
	else
		result = (*func)(rv1->data.double_value,rv2->data.double_value);

	if(result)
		pc = instr->result->value;
}

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

void execute_jeq (instr_s * instr){
	assert(instr->result->type == label_a);

	avm_memcell * rv1 = avm_translate_operand(instr->arg1,&ax);
	avm_memcell * rv2 = avm_translate_operand(instr->arg2,&bx);

	unsigned char result = 0;

	if(rv1->type == undefined_m)
		avm_error("Cannot perform equality, variable (",instr->arg1->name,") is undefined",instr->line);
	if(rv2->type == undefined_m)
		avm_error("Cannot perform equality, variable (",instr->arg2->name,") is undefined",instr->line);

	if(rv1->type == nil_m || rv2->type == nil_m)
		result = rv1->type == nil_m && rv2->type == nil_m;
	else if(rv1->type == bool_m || rv2->type == bool_m)
		result = (avm_tobool(rv1) == avm_tobool(rv2));
	else if(rv1->type != rv2->type && (!is_num_type(rv1->type)|| !is_num_type(rv2->type)))
		avm_error("Equality expression with",instr->arg1->name,"is illegal",instr->line);
	else{
		if(rv1->type==string_m)
			result = !strcmp(rv1->data.str_value,rv2->data.str_value);
		else{
				if(rv1->type == integer_m && rv2->type==integer_m)
					result = (rv1->data.int_value == rv2->data.int_value);
				else if(rv1->type == integer_m)
					result = ((double)rv1->data.int_value == rv2->data.double_value);
				else if(rv2->type == integer_m)
					result = (rv1->data.double_value == (double)rv2->data.int_value);
				else
					result = (rv1->data.double_value == rv2->data.double_value);
		}
	}

	if(instr->opcode==jeq_v && result)
		pc = instr->result->value;
	else if(instr->opcode==jne_v && !result)
		pc = instr->result->value;
}

char is_num_type(avm_memcell_t type){
	return (type == integer_m || type == double_m);
}