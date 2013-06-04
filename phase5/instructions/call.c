#include "call.h"

unsigned int current_func_size = 0;

void execute_call(instr_s * instr){
	avm_memcell * func = avm_translate_operand(instr->result,&ax);
	assert(func);
	avm_call_save_env();
	switch(func->type){
		case userfunc_m:{
			pc = user_funcs[func->data.func_value].address;
			if(pc>=AVM_ENDING_PC)
				pc = func->data.func_value;

			if(pc==0 && instructions[pc].opcode != funcenter_v)
				pc = func->data.func_value;

			assert(pc<AVM_ENDING_PC);
			assert(instructions[pc].opcode == funcenter_v);
			break;
		}
		case string_m: avm_call_libfunc(func->data.str_value); break;
		case libfunc_m: avm_call_libfunc(func->data.lib_func_value); break;
		default: {
			char * s = avm_tostring(func);
			char * error_msg = create_string(strlen(s)+50);
			avm_error("Cannot bind ",s," to function",instr->line);
		}
	}
}

void avm_push_envvalue(unsigned int value){
	stack[top].type = integer_m;
	stack[top].data.int_value = value;
	avm_dec_top();
}

void avm_call_save_env(void){
	avm_push_envvalue(total_actuals);
	avm_push_envvalue(pc+1);
	avm_push_envvalue(top+total_actuals+2);
	avm_push_envvalue(topsp);
}

void avm_dec_top(void){
	if(top==0)
		avm_anonymous_error("Stack overflow");
	else 
		top--;
}

void execute_funcenter(instr_s * instr){
	avm_memcell * func = avm_translate_operand(instr->result,&ax);
	assert(func);
	assert(pc == func->data.func_value);
	total_actuals = 0;
	userfunc_s * func_info = avm_get_func_info(pc);
	topsp = top;
	top = top - func_info->local_size - avm_total_actuals();
	current_func_size = func_info->local_size;
}

userfunc_s * avm_get_func_info(unsigned int index){
	unsigned int func_index = instructions[pc].result->value;
	return(&user_funcs[func_index]);
}

unsigned int avm_get_env_value(unsigned int i){
	assert(stack[i].type = integer_m);
	unsigned int value = (unsigned int) stack[i].data.int_value;
	return value;
}

void execute_funcexit(instr_s * instr){
	unsigned old_top = top;
	top = avm_get_env_value(topsp + AVM_SAVEDTOP_OFFSET);
	pc = avm_get_env_value(topsp + AVM_SAVEDPC_OFFSET);
	topsp = avm_get_env_value(topsp + AVM_SAVEDTOPSP_OFFSET);

	while(old_top++ <= top)
		avm_clear_memcell(&stack[old_top]);
}

void avm_call_libfunc(char * id){
	if(!avm_library_func_exist(id))
		avm_anonymous_error("Unsupported lib func called");
	topsp = top;
	total_actuals = 0;
	
	if(strcmp(id,"print")==0)
		libfunc_print();
	else if(strcmp(id,"totalarguments")==0)
		libfunc_totalarguments();
	else if(strcmp(id,"argument")==0)
		libfunc_argument();
	else if(strcmp(id,"typeof")==0)
		libfunc_typeof();
	else if(strcmp(id,"input")==0)
		libfunc_input();
	else if(strcmp(id,"sin")==0)
		libfunc_sin();
	else if(strcmp(id,"cos")==0)
		libfunc_cos();
	else if(strcmp(id,"sqrt")==0)
		libfunc_sqrt();
	else if(strcmp(id,"strtonum")==0)
		libfunc_strtonum();
	else if(strcmp(id,"objecttotalmembers")==0)
		libfunc_objecttotalmembers();
	else if(strcmp(id,"objectmemberkeys")==0)
		libfunc_objectmemberkeys();
	else if(strcmp(id,"objectcopy")==0)
		libfunc_objectcopy();

	if(!execution_finished)
		execute_funcexit((instr_s *)NULL);
}

unsigned int avm_total_actuals(void){
	return avm_get_env_value(topsp+AVM_NUMACTUALS_OFFSET);
}

avm_memcell * avm_get_actual(unsigned int i){
	assert(i<avm_total_actuals());
	return &stack[topsp+AVM_STACK_ENV_SIZE+1+i];
}

unsigned char avm_library_func_exist(char * funcname){
	unsigned int i;
	for(i=0;i<total_named_lib_funcs;i++){
		if(strcmp(funcname,named_lib_funcs[i])==0)
			return 1;
	}
	return 0;
}

void execute_pusharg(instr_s * instr){
	avm_memcell * arg = avm_translate_operand(instr->result,&ax);
	if(arg->type==undefined_m)
		avm_warning("Undefined variable (",instr->result->name,") has been used as a function argument",instr->line);
	assert(arg);
	avm_assign(&stack[top],arg);
	total_actuals++;
	avm_dec_top();
}

void printstack(){
	unsigned int i;
	for(i=0;i<AVM_STACKSIZE;i++){
		if(stack[i].type!=undefined_m){
			printf("%d: %s\n",i,avm_tostring(&stack[i]));
		}
	}
	printf("\n------------------\n");
}