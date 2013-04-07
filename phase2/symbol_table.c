#include "symbol_table.h"

int st_insert(symbol_table ** st, st_entry ** symbol){
	unsigned int key = generate_key((*symbol)->name);
	scope_entry * temp = (*st)->scope_list;
	st_entry * symbol_cpy = create_symbol((*symbol)->name,(*symbol)->active,(*symbol)->scope,(*symbol)->line,(*symbol)->type);

	if((*st)->hash_table[key]==NULL)(*st)->hash_table[key] = *symbol;
	else{
		(*symbol)->next = (*st)->hash_table[key];
		(*st)->hash_table[key] = *symbol;
	} 
	
	while(temp!=NULL && temp->scope!=(*symbol)->scope)
		temp = temp->next;

	if(temp==NULL){
		temp = (scope_entry *)malloc(sizeof(scope_entry));
		if(memerror(temp,"scope entry"))return 0;
		temp->next = (*st)->scope_list;
		temp->scope = (*symbol)->scope;
		(*st)->scope_list = temp;
		(*st)->scope_list->symbols = symbol_cpy;
	}
	else{
		symbol_cpy->next = temp->symbols;
		temp->symbols = symbol_cpy;
	}

	return 1;
}

// Needs work
symbol_table * create_symbol_table(){
	int i;
	symbol_table * st = (symbol_table *)malloc(sizeof(symbol_table));
	if(memerror(st,"symbol table"))return NULL;

	for(i=0;i<BUCKET_SIZE;i++)
		st->hash_table[i] = NULL;
	
	st->scope_list = NULL;
	return st;
}

// Needs work
void print_st(symbol_table * st){
	int i;
	st_entry * entry;
	scope_entry * sc;

	for(i=0;i<BUCKET_SIZE;i++){
		entry = st->hash_table[i];
		while(entry!=NULL){
			printf("Symbol : %s\n",entry->name);
			entry = entry->next;
		}
	}
	printf("________\n");
	sc = st->scope_list;
	while(sc!=NULL){
		printf("SCOPE : %d\n",sc->scope);
		entry = sc->symbols;
		while(entry!=NULL){
			printf("Symbol : %s scope:%d\n",entry->name,entry->scope);
			entry = entry->next;
		}
		sc = sc->next;
	}

}

st_entry * st_lookup_table(symbol_table * st,const char * symbol_name){
	unsigned int key = generate_key(symbol_name);
	st_entry * temp = st->hash_table[key];
	while(temp!=NULL){
		if(strcmp(temp->name,symbol_name)==0)return temp;
		temp = temp->next;
	}
	return temp;
}

st_entry * st_lookup_scope(symbol_table * st,const char * symbol_name,unsigned int scope){
	st_entry * st_temp;
	scope_entry * sc_temp = st->scope_list;

	while(sc_temp!=NULL && sc_temp->scope!=scope)
		sc_temp = sc_temp->next;

	if(sc_temp==NULL)return NULL;
	st_temp = sc_temp->symbols;
	 
	printf("I GOT HERE WITH SCOPE %d\n",scope);
	while(st_temp!=NULL){
		if(strcmp(st_temp->name,symbol_name)==0)return st_temp;
		st_temp = st_temp->next;
	}

	return NULL;
}

st_entry * create_symbol(const char * name, unsigned int active, unsigned int scope,unsigned int line,st_entry_type type){
	st_entry * symbol = (st_entry *)malloc(sizeof(st_entry));
	if(memerror(symbol,"symbol"))return NULL;

	symbol->name = malloc(strlen(name)+1);
	if(memerror(symbol->name,"symbol:name"))
		return NULL;

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

st_entry * set_var_func(st_entry * symbol,const char * func_name){
	if(symbol->type==USERFUNC || symbol->type==LIBFUNC){
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
	if(memerror(arg,"func arg"))return 0;

	arg->name = malloc(strlen(arg_name)+1);
	if(memerror(arg->name,"func arg:name"))return 0;
	strcpy(arg->name,arg_name);

	if(*args==NULL){
		arg->next = NULL;
		*args = arg;
	}
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
	(*symbol)->active = ~hidden;
}