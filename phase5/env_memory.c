#include "env_memory.h"
 
/* The main registers of the AVM */
avm_memcell ax, bx, cx;
avm_memcell retval;

/* The stack pointers */
// Be careful here
unsigned int total_actuals = 0;
unsigned int top = AVM_STACKSIZE-1;
unsigned int topsp;

avm_memcell stack[AVM_STACKSIZE];

tostring_func_t to_str_funcs[] = {
	double_tostring,
	int_tostring,
	string_tostring,
	bool_tostring,
	table_tostring,
	userfunc_tostring,
	libfunc_tostring,
	nil_tostring,
	undef_tostring,
}; 

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
	fprintf(stdout,"\n\x1b[33mWarning : %s %s %s at line (%d).\e[0m\n",msg1,msg2,msg3,line);
}

void avm_error(char * msg1,char * msg2,char * msg3, unsigned int line){
	fprintf(stdout,"\n\x1b[31mRuntime error : %s %s %s at line (%d). \e[0m \n",msg1,msg2,msg3,line);
	fprintf(stdout,"\nThe program has exited with return code (0: BAD CODE).\n");
	exit(0);
}

void avm_anonymous_error(char * msg){
	fprintf(stdout,"\n\x1b[31mRuntime error : %s\e[0m\n",msg);
	fprintf(stdout,"\nThe program has exited with return code (0: BAD CODE).\n");
	exit(0);
}

char * string_tostring(avm_memcell *m){
	return(m->data.str_value);
}

char * avm_tostring(avm_memcell * m){
	return (*to_str_funcs[m->type])(m);
}

char * double_tostring(avm_memcell * m){
	char * output = create_string(100);
	sprintf(output,"%lf",m->data.double_value);
	return output;
}

char * int_tostring(avm_memcell * m){
	char * output = create_string(100);
	sprintf(output,"%d",m->data.int_value);
	return output;	
}

char * table_tostring(avm_memcell * m){
	char * output = create_string(1);
	avm_table_bucket * temp;
	unsigned int i;

	printf("[");
	for(i=0;i<AVM_TABLE_HASHSIZE;i++){
		temp = m->data.table_value->str_indexed[i];
		while(temp!=NULL){
			if(temp->value->type!=table_m)
				printf("{'%s':%s},",temp->key->data.str_value,avm_tostring(temp->value));
			else{
				printf("{'%s':",temp->key->data.str_value);
				avm_tostring(temp->value);
				printf("},");
			}
			temp = temp->next;
		}
	}

	for(i=0;i<AVM_TABLE_HASHSIZE;i++){
		temp = m->data.table_value->num_indexed[i];
		while(temp!=NULL){
			if(temp->value->type!=table_m)
				printf("{%d:%s},",temp->key->data.int_value,avm_tostring(temp->value));
			else{
				printf("{%d:",temp->key->data.int_value);
				avm_tostring(temp->value);
				printf("},");
			}
			temp = temp->next;
		}
	}

	printf("]");

	return output;	
}

char * userfunc_tostring(avm_memcell * m){
	return(user_funcs[m->data.func_value].name);
}

char * libfunc_tostring(avm_memcell * m){
	return(m->data.lib_func_value);
}

char * nil_tostring(avm_memcell * m){
	char * output = create_string(strlen("(nil)")+1);
	strcpy(output,"(nil)");
	return output;
}

char * undef_tostring(avm_memcell * m){
	char * output = create_string(strlen("<undefined>")+1);
	strcpy(output,"<undefined>");
	return output;
}

char * bool_tostring(avm_memcell * m){
	char * output;
	if(m->data.bool_value==1){
		output = create_string(strlen("true")+1);
		strcpy(output,"true");
	}
	else{
		output = create_string(strlen("false")+1);
		strcpy(output,"false");
	}
	return output;	
}

char * create_string(unsigned int len){
	char * string = malloc(len);
	memerror(string,"new strng");
	memset(string,'\0',len);
	return string;
}

avm_memcell * create_memcell(){
	avm_memcell * new_cell = malloc(sizeof(avm_memcell));
	new_cell->type = undefined_m;
	memerror(new_cell,"new avmcell");
	return new_cell;
}

void avm_init_stack(void){
	unsigned int i;
	for(i=0;i<AVM_STACKSIZE;i++){
		AVM_WIPEOUT(stack[i]);
		stack[i].type = undefined_m;
	}
	memerror(stack,"avm stack");
}

void avm_table_bucket_init(avm_table_bucket ** bucket){
	unsigned int i;
	for(i=0;i<AVM_TABLE_HASHSIZE;i++){
		bucket[i] = (avm_table_bucket *) NULL;
	}
}

avm_table * avm_newtable(void){
	avm_table * table = (avm_table *)malloc(sizeof(avm_table));
	memerror(table,"new table");
	AVM_WIPEOUT(*table);

	table->ref_counter = 1;
	table->total = 0;
	avm_table_bucket_init(table->num_indexed);
	avm_table_bucket_init(table->str_indexed);
	return table;
}

void avm_destorytable(avm_table * table){
	avm_table_bucket_destroy(table->str_indexed);
	avm_table_bucket_destroy(table->num_indexed);
	free(table);
}

void avm_table_incr_refcounter(avm_table * table){
	table->ref_counter++;
}

void avm_table_decr_refcounter(avm_table * table){
	assert(table->ref_counter>0);
	table->ref_counter--;
	if(table->ref_counter==0)
		avm_destorytable(table);
}
 
void avm_clear_memcell(avm_memcell * cell){
	if(cell->type!=undefined_m){
		if(cell->type==string_m && cell->data.str_value!=NULL)
			free(cell->data.str_value);
		else if(cell->type==table_m)
			avm_table_decr_refcounter(cell->data.table_value);
		cell->type = undefined_m;
	}
}

void avm_table_bucket_destroy(avm_table_bucket ** bucket){
	unsigned int i;
	avm_table_bucket * del_b;
	for(i=0;i<AVM_TABLE_HASHSIZE;i++){
		while(bucket[i]){
			del_b = bucket[i];
			bucket[i] = bucket[i]->next;
			avm_clear_memcell(del_b->value);
			free(del_b->value);
			free(del_b->key);
			free(del_b);
		}
		bucket[i] = NULL;
	}
}
 
char * value_type_to_str(avm_memcell_t type){
	char * value_types[] = {"double","integer","string","boolean",
							"table","user function","library function",
							"nil","undefined"};
	return(value_types[type]);
} 

char * arg_value_type_to_str(vmarg_t type){
	char * value_types[] = {"label_a","global_a","formal_a","local_a",
							"integer_a" ,"double_a" ,"string_a" ,"bool_a",
							"nil_a" , "userfunc_a" ,"libfunc_a" ,"retval_a"};
	return(value_types[type]);
} 

char * real_value_type_to_str(avm_memcell_t type){
	char * value_types[] = {"number","number","string","boolean",
							"table","user function","library function",
							"nil","undefined"};
	return(value_types[type]);
} 