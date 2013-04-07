#include "symbol_table.h"


int st_insert(symbol_table ** st, st_entry ** symbol){
	int key = generate_key((*symbol)->name);
	if((*st)->hash_table[key]==NULL)(*st)->hash_table[key] = *symbol;
	else {
		(*symbol)->next = (*st)->hash_table[key];
		(*st)->hash_table[key] = *symbol;
	} 
	return 1;
}

symbol_table * create_symbol_table()
{
	int i;
	symbol_table * st = (symbol_table *)malloc(sizeof(symbol_table));
	if(memerror(st,"symbol table"))return NULL;

	for(i=0;i<BUCKET_SIZE;i++)st->hash_table[i] = NULL;
	st->scope_list = NULL;

	return st;
}

st_entry * create_symbol(const char * name, unsigned int active, unsigned int scope,unsigned int line,st_entry_type type)
{
	st_entry * symbol = (st_entry *)malloc(sizeof(st_entry));
	if(memerror(symbol,"symbol"))return NULL;

	symbol->name = malloc(strlen(name)+1);
	if(memerror(symbol->name,"symbol:name"))return NULL;

	strcpy(symbol->name,name);
	symbol->active = active;
	symbol->scope = scope;
	symbol->line = line;
	symbol->type = type;

	if(type==USERFUNC || type==LIBFUNC){
		symbol->value_type.funVal = (function *)malloc(sizeof(function));
		if(memerror(symbol->value_type.funVal,"funval"))return NULL;
		symbol->value_type.funVal->arguments = NULL;
	}
	else {
		symbol->value_type.varVal = (variable *)malloc(sizeof(variable));
		if(memerror(symbol->value_type.varVal,"varVal"))return NULL;
		symbol->value_type.varVal->used_in_func = NULL;
	}

	symbol->next = NULL;
	return symbol;
}

st_entry * set_var_func(st_entry * symbol,const char * func_name)
{
	if(symbol==NULL)return symbol;
	else if(symbol->type==USERFUNC || symbol->type==LIBFUNC){
		printf("Error : Symbol (%s) varVal not initialized, is it a function?\n",symbol->name);
		return symbol;
	}

	symbol->value_type.varVal->used_in_func = malloc(strlen(func_name)+1);
	if(memerror(symbol->value_type.varVal->used_in_func,"user_in_funct"))return NULL;

	strcpy(symbol->value_type.varVal->used_in_func,func_name);
	return symbol;
}

int memerror(void * ptr, const char * name){
	if(ptr==NULL){
		printf("Error : Memory allocation for the <%s> failed.\n",name);
		return 1;
	}
	return 0;
}

int generate_key(const char * name){
	int sum = 0;
	int len = strlen(name);
	int i;

	for(i=0;i<len;i++)
		sum += (int)name[i];

	return sum % BUCKET_SIZE;
}

int args_insert(arg_node ** args,const char * arg_name){
	if(args_lookup(*args,arg_name)!=NULL)return 0;

	arg_node * arg = (arg_node *)malloc(sizeof(arg_node));
	if(memerror(arg,"func arg"))return;

	arg->name = malloc(strlen(arg_name)+1);
	if(memerror(arg->name,"func arg:name"))return;

	if(*args==NULL)*args = arg;
	else{
		arg->next = *args;
		*args = arg;
	}
	return 1;
}

arg_node * args_lookup(arg_node * args,const char * arg_name){
	arg_node * tmp = args;
	while(tmp!=NULL){
		if(strcmp(tmp->name,arg_name)==0)return tmp;
		tmp = tmp->next;
	}
	return NULL;
}

void symbol_set_hidden(st_entry ** symbol,const char hidden){
	if(hidden)(*symbol)->active = 0;
	else (*symbol)->active = 1;
}

void print_st(symbol_table * st){
	int i;
	st_entry * entry;

	for(i=0;i<BUCKET_SIZE;i++){
		entry = st->hash_table[i];
		while(entry!=NULL){
			printf("Symbol : %s\n",entry->name);
			entry = entry->next;
		}
	}
}