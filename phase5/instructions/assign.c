#include "assign.h"

void execute_assign(instr_s * instr){
	avm_memcell * lv = avm_translate_operand(instr->result,(avm_memcell *)NULL);
	avm_memcell * rv = avm_translate_operand(instr->arg1, &ax);
	avm_assign(lv,rv,instr->line); 
}

void avm_assign(avm_memcell * lv, avm_memcell * rv,unsigned int line){
	if(lv==rv)
		return;

	if(lv->type == table_m && rv->type == table_m && lv->data.table_value == rv->data.table_value)
		return;

	if(rv->type == undefined_m )
		avm_warning("Assigning from undefined variable (",instructions[pc].result->name,")",instructions[pc].line);

	avm_clear_memcell(lv);

	assert(rv->type>=double_m && rv->type<=undefined_m);
	
	memcpy(lv,rv,sizeof(avm_memcell));

	if(lv->type == string_m)
		lv->data.str_value = strdup(rv->data.str_value);
	else if(lv->type==table_m)
		avm_table_incr_refcounter(lv->data.table_value);
}