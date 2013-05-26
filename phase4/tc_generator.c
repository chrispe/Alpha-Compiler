#include "tc_generator.h"

/* The expandable arrays for the constants of the code */

// For the number constants 
double * double_consts = NULL;
unsigned int current_double_index = 0;
unsigned int total_double_consts = 0;

// For the integer number constants
int * integer_consts = NULL;
unsigned int current_int_index = 0;
unsigned int total_integer_consts = 0;

// For the strings
char ** str_consts = NULL;
unsigned int current_str_index = 0;
unsigned int total_str_consts = 0;

// For the library functions
char ** named_lib_funcs = NULL;
unsigned int current_lib_func_index = 0;
unsigned int total_named_lib_funcs = 0;

// For the user functions
userfunc_s * user_funcs = NULL;
unsigned int current_user_func_index = 0;
unsigned int total_user_funcs = 0;

/* The list for the incomplete jumps */
incomplete_jump * i_jumps = NULL;
unsigned int i_jumps_total = 0;

/* The array that will include 
   the final target instructions. */
instr_s * instructions = NULL;
unsigned int current_instr_index = 0;
unsigned int total_instructions = 0;

unsigned int add_const_to_array(void * constant,const_t type){
	unsigned int value_index;
	value_index = value_exists_in_arr(constant,type);
	if(value_index!=-1)
		return(value_index);

	switch(type){
		case double_c:{
			if(current_double_index == total_double_consts)
				expand_const_array(type);
			double * current_double = double_consts + current_double_index++;
			*current_double = *((double *)(constant));
			break;
		}
		case int_c:{
			if(current_int_index == total_integer_consts)
				expand_const_array(type);
			int * current_int = integer_consts + current_int_index++;
			*current_int = *((int *)(constant));
			break;
		}
		case str_c:{
			if(current_str_index == total_str_consts)
			expand_const_array(type);
			char *  current_str = malloc(strlen((char*)constant)+1);
			strcpy(current_str,(char*)constant);
			str_consts[current_str_index++] = current_str;
			break;
		}
		case user_func_c:{
			if(current_user_func_index == total_user_funcs)
				expand_const_array(type);
			userfunc_s * current_user_func = user_funcs + current_user_func_index++;
			current_user_func->address = ((userfunc_s *)constant)->address;
			current_user_func->local_size = ((userfunc_s *)constant)->local_size;
			current_user_func->name = malloc(strlen(((userfunc_s *)constant)->name)+1);
			strcpy(current_user_func->name,((userfunc_s *)constant)->name);
			break;
		}
		case lib_func_c:{
			if(current_lib_func_index == total_named_lib_funcs)
				expand_const_array(type);
			char * current_lib_func = malloc(strlen((char *)constant)+1);
			strcpy(current_lib_func,(char *)constant);
			named_lib_funcs[current_lib_func_index++] = current_lib_func;
			break;
		}
		default: assert(0);
	}
}

unsigned int value_exists_in_arr(void * value, const_t type){
	int i;
	switch(type){
		case double_c:{
			for(i=0;i<current_double_index;i++){
				if(double_consts[i]==*((double *)value))return i;
			}
			break;
		}
		case int_c:{
			for(i=0;i<current_int_index;i++){
				if(integer_consts[i]==*((int *)value))return i;
			}
			break;
		}
		case str_c:{
			for(i=0;i<current_str_index;i++){
				if(strcmp(str_consts[i],(char *)value)==0)return i;
			}
			break;
		}
		case user_func_c:{
			for(i=0;i<current_user_func_index;i++){
				if(strcmp((user_funcs[i]).name,(char *)value)==0)return i;
			}
			break;
		}
		case lib_func_c:{
			for(i=0;i<current_lib_func_index;i++){
				if(strcmp(named_lib_funcs[i],(char *)value)==0)return i;
			}
			break;
		}
		default: assert(0);
	}
	return -1;
}

void expand_const_array(const_t array_type){
	switch(array_type){
		case double_c:{
			double * new_double_arr = (double *)malloc(DOUBLE_ARR_NEW_SIZE);
			if(memerror(new_double_arr,"new double array"))exit(0);
			memcpy(new_double_arr,double_consts,DOUBLE_ARR_SIZE);
			double_consts = new_double_arr;
			total_double_consts += EXPAND_SIZE;
			break;
		}
		case int_c:{
			int * new_int_arr = (int *)malloc(INTGER_ARR_NEW_SIZE);
			if(memerror(new_int_arr,"new integer array"))exit(0);
			memcpy(new_int_arr,integer_consts,INTEGER_ARR_SIZE);
			integer_consts = new_int_arr;
			total_integer_consts += EXPAND_SIZE;
			break;
		}
		case str_c:{
			char ** new_str_arr = (char **)malloc(STRING_ARR_NEW_SIZE);
			if(memerror(new_str_arr,"new string array"))exit(0);
			memcpy(new_str_arr,str_consts,STRING_ARR_SIZE);
			str_consts = new_str_arr;
			total_str_consts += EXPAND_SIZE;
			break;
		}
		case user_func_c:{
			userfunc_s * new_user_func_arr = (userfunc_s *)malloc(USER_FUNC_ARR_NEW_SIZE);
			if(memerror(new_user_func_arr,"new user func array"))exit(0);
			memcpy(new_user_func_arr,user_funcs,USER_FUNC_ARR_SIZE);
			user_funcs = new_user_func_arr;
			total_user_funcs += EXPAND_SIZE;
			break;
		}
		case lib_func_c:{
			char ** new_lib_func_arr = (char **)malloc(LIB_FUNC_ARR_NEW_SIZE);
			if(memerror(new_lib_func_arr,"new lib func array"))exit(0);
			memcpy(new_lib_func_arr,named_lib_funcs,USER_FUNC_ARR_SIZE);
			named_lib_funcs = new_lib_func_arr;
			total_named_lib_funcs += EXPAND_SIZE;
			break;
		}
		default: assert(0);
	}
}

vmarg_s * create_vmarg(void){
	vmarg_s * new_vmarg = malloc(sizeof(vmarg_s));
	if(memerror(new_vmarg,"new vmarg"))exit(0);
	return(new_vmarg);
}
 
instr_s * create_instr(void){
	instr_s * new_instr = malloc(sizeof(instr_s));
	if(memerror(new_instr,"new instr"))exit(0);
	new_instr->arg1 = NULL;
	new_instr->arg2 = NULL;
	new_instr->result = NULL;
	return(new_instr);
}

void make_operand(expr * e, vmarg_s * arg){
	switch(e->type){
		case var_e:
		case table_item_e:
		case arithm_expr_e:
		case bool_expr_e:
		case new_table_e: {
			assert(e->sym);
			arg->value = e->sym->offset;
			switch(e->sym->space){
				case PROGRAM_VAR: arg->type = global_a;	break;
				case FUNC_LOCAL:  arg->type = local_a;	break;
				case FORMAL_ARG:  arg->type = formal_a;	break;
				default:assert(0);
			}
			break;
		}
		case const_bool_e:{
			arg->value = e->bool_value;
			arg->type = bool_a;
			break;
		}
		case const_str_e:{
			arg->value = add_const_to_array(e->str_value,str_c);
			arg->type = string_a;
			break;
		}
		case const_int_e:{
			arg->value = add_const_to_array(&(e->int_value),int_c);
			arg->type = integer_a;
			break;
		}
		case const_num_e:{
			arg->value = add_const_to_array(&(e->num_value),double_c);
			arg->type = double_a;
			break;
		}
		case nil_e:{
			arg->type = nil_a;
			break;
		}
		case program_func_e:{
			arg->type = userfunc_a;
			arg->value = e->sym->taddress;
			break;
		}
		case library_func_e:{
			arg->type = libfunc_a;
			arg->value = add_const_to_array(e->sym->name,lib_func_c);
			break;
		}
		default: assert(0);
	}
}

void make_double_operand(vmarg_s * arg,double * value){
	arg->value = add_const_to_array(value,double_c);
	arg->type = double_a;
}

void make_integer_operand(vmarg_s * arg,int value){
	int c = value;
	arg->value = add_const_to_array(&c,int_c);
	arg->type = integer_a;
}

void make_boolean_operand(vmarg_s * arg,unsigned int value){
	arg->value = value;
	arg->type = bool_a;
}

void make_retval_operand(vmarg_s * arg){
	arg->type = retval_a;
}

void add_incomplete_jump(unsigned int instr_id, unsigned int iaddress){
	incomplete_jump * new_jump = malloc(sizeof(incomplete_jump));
	new_jump->instr_id = instr_id;
	new_jump->iaddress = iaddress;
	new_jump->next = i_jumps;
	i_jumps = new_jump;
}

void emit_instruction(vmopcode_e op,vmarg_s * arg1,vmarg_s *arg2, vmarg_s * result,unsigned int line){
	instr_s * new_instr = create_instr();
	if(current_instr_index == total_instructions)
		expand_instr_array();

	new_instr = instructions + current_instr_index++;
	new_instr->opcode = op;
	new_instr->arg1 = arg1;
	new_instr->arg2 = arg2;
	new_instr->result = result;
	new_instr->line = line;	
}

void expand_instr_array(void){
	instr_s * new_instr_arr = (instr_s *)malloc(INSTR_ARR_NEW_SIZE);
	if(memerror(new_instr_arr,"new instruction array"))exit(0);
	memcpy(new_instr_arr,instructions,INSTR_ARR_SIZE);
	instructions = new_instr_arr;
	total_instructions += EXPAND_SIZE;
}

void patch_incomplete_jumps(void){
	incomplete_jump * temp = i_jumps;
	while(temp){
		if(temp->iaddress == curr_quad)
			instructions[temp->instr_id].result->value = next_instr_label();
		else
			instructions[temp->instr_id].result->value = quads[temp->iaddress].taddress;
		temp = temp->next;
	}
}

unsigned int next_instr_label(void){
	return(current_instr_index);
}

void emit_instruction_s(instr_s * instr){
	instr_s * new_instr = create_instr();

	if(current_instr_index == total_instructions)
		expand_instr_array();

	new_instr = instructions + current_instr_index++;
	new_instr->opcode = instr->opcode;
	new_instr->arg1 = instr->arg1;
	new_instr->arg2 = instr->arg2;
	new_instr->result = instr->result;
	new_instr->line = instr->line;	
}

void generate(vmopcode_e op, quad * q){
	instr_s * instr = create_instr();
	instr->arg1  = create_vmarg();
	instr->arg2  = create_vmarg();
	instr->result = create_vmarg();

	if(q->arg1)
		make_operand(q->arg1,instr->arg1);
	if(q->arg2)
		make_operand(q->arg2,instr->arg2);
	if(q->result)
		make_operand(q->result,instr->result);

	instr->line = q->line;
	q->taddress = next_instr_label();
	emit_instruction_s(instr);
	destory_instr(instr);
}

void generate_ADD(quad * q){
	generate(add_v,q);
}

void generate_SUB(quad * q){
	generate(sub_v,q);
}

void generate_MUL(quad * q){
	generate(mul_v,q);
}

void generate_DIV(quad * q){
	generate(div_v,q);
}

void generate_MOD(quad * q){
	generate(mod_v,q);
}

void generate_UMINUS(quad * q){
	instr_s * instr = create_instr();
	instr->arg1 = create_vmarg();
	instr->arg2 = create_vmarg();
	instr->result = create_vmarg();
	instr->opcode = mul_v;
	instr->line = q->line;

	q->taddress = next_instr_label();
	
	make_operand(q->arg1,instr->arg1);
	make_integer_operand(instr->arg2,-1);
	make_operand(q->result,instr->result);
	emit_instruction_s(instr);
	destory_instr(instr);
}

void generate_NEWTABLE(quad * q){
	generate(newtable_v,q);
}

void generate_TABLEGETELEM(quad * q){
	generate(tablegetelem_v,q);
}

void generate_TABLESETELEM(quad * q){
	generate(tablesetelem_v,q);
}

void generate_ASSIGN(quad * q){
	generate(assign_v,q);
}

void generate_NOP(void){
	instr_s * instr = create_instr();
	instr->opcode = nop_v;
	emit_instruction_s(instr);
	destory_instr(instr);
}

void generate_relational(vmopcode_e op, quad * q){
	vmarg_s * result = create_vmarg();
	result->type = label_a;

	instr_s * instr = create_instr();
	instr->arg1 = create_vmarg();
	instr->arg2 = create_vmarg();
	instr->opcode = op;
	instr->line = q->line;
	instr->result = result;

	if(q->arg1)
		make_operand(q->arg1,instr->arg1);
	
	if(q->arg2)
		make_operand(q->arg2,instr->arg2);

	if(q->label < curr_quad)
		result->value = quads[q->label].taddress;
	else
		add_incomplete_jump(next_instr_label(),q->label);
	
	q->taddress = next_instr_label();
	emit_instruction_s(instr);
	destory_instr(instr);
}

void generate_JUMP(quad * q){
	generate_relational(jump_v,q);
}

void generate_IF_EQ(quad * q){
	generate_relational(jeq_v,q);
}

void generate_IF_NOTEQ(quad * q){
	generate_relational(jne_v,q);
}

void generate_IF_GREATER(quad * q){
	generate_relational(jgt_v,q);
}

void generate_IF_GREATEREQ(quad * q){
	generate_relational(jge_v,q);
}

void generate_LESS(quad * q){
	generate_relational(jlt_v,q);
}

void generate_LESSEQ(quad * q){
	generate_relational(jle_v,q);
}

void generate_NOT(quad * q){
	q->taddress = next_instr_label();

	instr_s * instr = create_instr();
	instr->opcode = jeq_v;

	instr->arg1 = create_vmarg();
	instr->arg2 = create_vmarg();
	instr->result = create_vmarg();
	make_operand(q->arg1,instr->arg1);
	make_boolean_operand(instr->arg2,0);
	instr->result->type = label_a;
	instr->result->value = next_instr_label() + 3;
	emit_instruction_s(instr);

	instr->opcode = assign_v;
	make_boolean_operand(instr->arg1,0);
	reset_operand(instr->arg2);
	make_operand(q->result,instr->result);
	emit_instruction_s(instr);

	instr->opcode = jump_v;
	reset_operand(instr->arg1);
	reset_operand(instr->arg2);
	instr->result->type = label_a;
	instr->result->value = next_instr_label() + 2;
	emit_instruction_s(instr);

	instr->opcode = assign_v;
	instr->arg1 = create_vmarg();
	make_boolean_operand(instr->arg1,1);
	reset_operand(instr->arg2);
	make_operand(q->result,instr->result);
	emit_instruction_s(instr);
	destory_instr(instr);
}

void generate_OR(quad * q){
	q->taddress = next_instr_label();
	instr_s * instr = create_instr();
	instr->arg1 = create_vmarg();
	instr->arg2 = create_vmarg();
	instr->result = create_vmarg();

	instr->opcode = jeq_v;
	make_operand(q->arg1,instr->arg1);
	make_boolean_operand(instr->arg2,1);
	instr->result->type = label_a;
	instr->result->value = next_instr_label() + 4;
	emit_instruction_s(instr);

	make_operand(q->arg2,instr->arg1);
	instr->result->value = next_instr_label() + 3;
	emit_instruction_s(instr);

	instr->opcode = assign_v;
	make_boolean_operand(instr->arg1,0);
	reset_operand(instr->arg2);
	make_operand(q->result,instr->result);
	emit_instruction_s(instr);

	instr->opcode = jump_v;
	reset_operand(instr->arg1);
	reset_operand(instr->arg2);
	instr->result->type = label_a;
	instr->result->value = next_instr_label() + 2;
	emit_instruction_s(instr);

	instr->opcode = assign_v;
	instr->arg1 = create_vmarg();
	instr->arg2 = create_vmarg();
	make_boolean_operand(instr->arg1,1);
	reset_operand(instr->arg2);
	make_operand(q->result,instr->result);
	emit_instruction_s(instr);
}

void generate_AND(quad * q){
	instr_s * instr = create_instr();
	q->taddress = next_instr_label();

	instr->arg1 = create_vmarg();
	instr->arg2 = create_vmarg();
	instr->result = create_vmarg();
	instr->opcode = jeq_v;
	make_operand(q->arg1,instr->arg1);
	make_boolean_operand(instr->arg2,0);
	instr->result->type = label_a;
	instr->result->value = next_instr_label() + 4;
	emit_instruction_s(instr);

	instr->opcode = jeq_v;
	make_operand(q->arg2,instr->arg1);
	make_boolean_operand(instr->arg2,0);
	instr->result->type = label_a;
	instr->result->value = next_instr_label() + 3;
	emit_instruction_s(instr);

	instr->line = q->line;
	instr->opcode = assign_v;
	make_boolean_operand(instr->arg1,1);
	make_operand(q->result,instr->result);
	emit_instruction_s(instr);

	instr->opcode = jump_v;
	reset_operand(instr->arg1);
	instr->result->type = label_a;
	instr->result->value = next_instr_label() + 2;
	instr->line = q->line;
	emit_instruction_s(instr);

	instr->opcode = assign_v;
	instr->line = q->line;
	instr->arg1 = create_vmarg();
	make_boolean_operand(instr->arg1,0);
	make_operand(q->result,instr->result);
	emit_instruction_s(instr);
}

void generate_PARAM(quad * q){
	q->taddress = next_instr_label();
	instr_s * instr = create_instr();
	instr->opcode = pusharg_v;
	instr->arg1 = create_vmarg();
	make_operand(q->arg1,instr->arg1);
	emit_instruction_s(instr);
	destory_instr(instr);
}

void generate_CALL(quad * q){
	q->taddress = next_instr_label();
	instr_s * instr = create_instr();
	instr->opcode = call_v;
	instr->arg1 = create_vmarg();
	make_operand(q->arg1,instr->arg1);
	emit_instruction_s(instr);
	destory_instr(instr);
}

void generate_GETRETVAL(quad * q){
	q->taddress = next_instr_label();
	instr_s * instr = create_instr();
	instr->opcode = assign_v;
	instr->result = create_vmarg();
	instr->arg1 = create_vmarg();
	make_operand(q->result,instr->result);
	make_retval_operand(instr->arg1);
	emit_instruction_s(instr);
	destory_instr(instr);
}

void reset_operand(vmarg_s * arg){
	free(arg);
	arg = NULL;
}

void destory_instr(instr_s * instr){
	free(instr->arg1);
	free(instr->arg2);
	free(instr->result);
	free(instr);
}

