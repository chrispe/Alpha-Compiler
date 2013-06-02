#include "memory_manager.h"

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
	char * output = create_string(strlen("table")+1);
	strcpy(output,"table");
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

	table->ref_counter = 0;
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
			avm_clear_memcell(del_b->key);
			free(del_b);
		}
		bucket[i] = NULL;
	}
}