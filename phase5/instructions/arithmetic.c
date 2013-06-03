#include "arithmetic.h"

arithmetic_func_t arithmetic_funcs[] = {
	add_impl,
	sub_impl,
	mul_impl,
	div_impl,
	mod_impl
};

double add_impl (double x, double y){
 	return x+y;
}

double sub_impl (double x, double y){
 	return x-y;
}

double mul_impl (double x, double y){
 	return x*y;
}

double div_impl (double x, double y){
 	return x/y;
 }

double mod_impl(double x, double y){
	int result = (int)(x/y);
	return x - (double)result * y;
}

void execute_arithmetic(instr_s * instr){
	avm_memcell * lv = avm_translate_operand(instr->result, (avm_memcell *)NULL);
	avm_memcell * rv1 = avm_translate_operand(instr->arg1, &ax);
	avm_memcell * rv2 = avm_translate_operand(instr->arg2, &bx);

	assert(rv1 && rv2);

	char rv1_valid = (rv1->type == integer_m || rv1->type== double_m);
	char rv2_valid = (rv2->type == integer_m || rv2->type== double_m);
 
	if(rv1_valid && rv2_valid){
		arithmetic_func_t op = arithmetic_funcs[instr->opcode-add_v];
		avm_clear_memcell(lv);
		lv->type = double_m;
		if(rv1->type == integer_m && rv2->type==integer_m)
			lv->data.double_value = (*op)((double)rv1->data.int_value,(double)rv2->data.int_value);
		else if(rv1->type == integer_m)
			lv->data.double_value = (*op)((double)rv1->data.int_value,rv2->data.double_value);
		else if(rv2->type == integer_m)
			lv->data.double_value = (*op)(rv1->data.double_value,(double)rv2->data.int_value);
		else
			lv->data.double_value = (*op)(rv1->data.double_value,rv2->data.double_value);
	}
	else{
		if(!rv1_valid)
			avm_error(instr->arg1->name," has invalid type, cannot perform arithmetic operations on a",value_type_to_str(rv1->type),instr->line);
		if(!rv2_valid)
			avm_error(instr->arg2->name," has invalid type, cannot perform arithmetic operations on a",value_type_to_str(rv2->type),instr->line);
	}
}

 