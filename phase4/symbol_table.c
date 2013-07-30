#include "symbol_table.h"

symbol_table * st = NULL;

int st_insert(symbol_table ** st, st_entry ** symbol) {
	unsigned int key = generate_key((*symbol)->name);
	scope_entry * temp = (*st)->scope_list;
	scope_entry * previous = NULL;

	// Making a copy of the symbol to link it to the scope list.
	st_entry * symbol_cpy = create_symbol((*symbol)->name,(*symbol)->active,(*symbol)->scope,(*symbol)->line,(*symbol)->type,(*symbol)->offset,(*symbol)->space);
	if((*symbol)->value_type.varVal->used_in_func != NULL)
		symbol_cpy =  set_var_func(symbol_cpy,(*symbol)->value_type.varVal->used_in_func);

	// Insertion in the hash table.
	if ((*st)->hash_table[key])
		(*symbol)->next = (*st)->hash_table[key];
	(*st)->hash_table[key] = *symbol;

	// Insertion in the scope list.
	while (temp && temp->scope<(*symbol)->scope){
		previous = temp;
		temp = temp->next;
	}

	if (temp == NULL || temp->scope != (*symbol)->scope) {
		temp = (scope_entry *)malloc(sizeof(scope_entry));
		temp->scope = (*symbol)->scope;
		if (memerror(temp,"scope entry"))
			return 0;

		if(previous!=NULL){
			temp->next = previous->next;
			previous->next = temp;
		}
		else {
			temp->next = (*st)->scope_list;
			(*st)->scope_list = temp;
		}
		temp->symbols = symbol_cpy;
	}
	else 
		symbol_cpy->next = temp->symbols;
	 
	temp->symbols = symbol_cpy;

	// Seting the pointer to the last symbol that was added
	if(symbol_cpy->type!=FORMAL)
		(*st)->last_symbol = symbol_cpy;

	return 1;
}

symbol_table * create_symbol_table(){
	char * lib_functions[] = {
					"print","input","objectmemberkeys","objecttotalmembers",
				  	"objectcopy","totalarguments","argument","typeof","strtonum",
				  	"sqrt","cos","sin" };
	
	unsigned int i;
	st_entry * symbol;
	symbol_table * st = (symbol_table *)malloc(sizeof(symbol_table));
	if(memerror(st,"symbol table"))
		return NULL;

	for(i=0;i<BUCKET_SIZE;i++)
		st->hash_table[i] = NULL;
	st->scope_list = NULL;

	// We add all the library functions from the beginning in the symbol table.
	for(i=0;i<12;i++){
		symbol = create_symbol(lib_functions[i],1,0,0,LIBFUNC,0,0);
		if(memerror(symbol,"initalize lib func"))
			return NULL;
		st_insert(&st,&symbol);
	}

	return st;
}
 
void print_st(symbol_table * st){
	st_entry * entry;
	scope_entry * sc;
	arg_node * arg;
	sc = st->scope_list;

	while(sc!=NULL){
		entry = sc->symbols;
		printf("Symbol Table scope:<%d>\n",sc->scope);
		while(entry!=NULL){
				printf("\tkey='%s' ",entry->name);
				if(entry->type==GLOBAL_VAR)printf("type=(global variable) ");
				else if(entry->type==VAR)printf("type=(variable) ");
				else if(entry->type==LCAL)printf("type=(local variable at : %s) ",entry->value_type.varVal->used_in_func);
				else if(entry->type==LIBFUNC)printf("type=(library function) ");
				else if(entry->type==FORMAL)printf("type=(formal at : %s)",entry->value_type.varVal->used_in_func);
				else if(entry->type==TEMP_VAR)printf("type=(temp variable at %s) ",entry->value_type.varVal->used_in_func);
				else{
					printf("user_function(");
					arg = entry->value_type.funVal->arguments;
					while(arg!=NULL){
						printf("%s,",arg->name);
						arg = arg->next;
					}
					printf(") ");
				}
				if(entry->space==PROGRAM_VAR)printf("space='program_var'");
				else if(entry->space==FUNC_LOCAL)printf("space='func_local'");
				else printf("space='func_arg'");
				printf(" line=%d scope=%d active=%d offset=%d\n",entry->line,entry->scope,entry->active,entry->offset);
				entry = entry->next;
		}
		printf("\n");
		sc = sc->next;
	}
}

st_entry * st_lookup_table(symbol_table * st,const char * symbol_name){
	unsigned int key = generate_key(symbol_name);
	st_entry * temp = st->hash_table[key];

	while(temp!=NULL){
		if(strcmp(temp->name,symbol_name)==0 && temp->active==1)
			return temp;
		temp = temp->next;
	}
	return temp;
}

st_entry * st_lookup_scope(symbol_table * st,const char * symbol_name,unsigned int scope){
	st_entry * st_temp;
	scope_entry * sc_temp = st->scope_list;

	while(sc_temp!=NULL && sc_temp->scope!=scope)
		sc_temp = sc_temp->next;

	if(sc_temp==NULL)
		return NULL;
	
	st_temp = sc_temp->symbols;
	 
	while(st_temp!=NULL){
		if(strcmp(st_temp->name,symbol_name)==0 && st_temp->active==1)
			return st_temp;
		st_temp = st_temp->next;
	}

	return NULL;
}

st_entry * create_symbol(const char * name, unsigned int active, unsigned int scope,unsigned int line,st_entry_type type, unsigned int offset, scopespace_t space){
	st_entry * symbol = (st_entry *)malloc(sizeof(st_entry));
	if(memerror(symbol,"symbol"))
		return NULL;

	symbol->name = malloc(strlen(name)+1);
	if(memerror(symbol->name,"symbol:name")){
		free(symbol);
		return NULL;
	}

	strcpy(symbol->name,name);
	symbol->active = active;
	symbol->scope = scope;
	symbol->line = line;
	symbol->type = type;
	symbol->offset = offset;
	symbol->space = space;

	// Depending on the symbol type we set the correct variables.
	if(type==USERFUNC || type==LIBFUNC){
		symbol->value_type.funVal = (function *)malloc(sizeof(function));
		if(memerror(symbol->value_type.funVal,"funval"))
		{
			free(symbol->name);
			free(symbol);
			return NULL;
		}
		symbol->value_type.funVal->arguments = NULL;
	}
	else {
		symbol->value_type.varVal = (variable *)malloc(sizeof(variable));
		if(memerror(symbol->value_type.varVal,"varVal")){
			free(symbol->name);
			free(symbol);
			return NULL;
		}
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
	if(memerror(symbol->value_type.varVal->used_in_func,"user_in_funct"))
		return NULL;
	
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

void memcheck(void * p, const char * name){
	if(memerror(p,name))
		exit(0);
}

int generate_key(const char * name){
	unsigned int sum, i;
	unsigned int len = strlen(name);

	for(i=0;i<len;i++)
		sum += (int)name[i];

	return sum % BUCKET_SIZE;
}

int args_insert(arg_node ** args,const char * arg_name){
	arg_node * temp = *args;

	if(args_lookup(*args,arg_name)!=NULL)
		return 0;

	arg_node * arg = (arg_node *)malloc(sizeof(arg_node));
	if(memerror(arg,"func arg"))
		return 0;

	arg->name = malloc(strlen(arg_name)+1);
	if(memerror(arg->name,"func arg:name")){
		free(arg);
		return 0;
	}
	strcpy(arg->name,arg_name);

	arg->next = NULL;

	if(*args==NULL)
		*args = arg;
	else {
		while((*args)->next!=NULL)
			*args = (*args)->next;
		(*args)->next = arg;
		*args = temp;
	}

	return 1;
}

arg_node * args_lookup(arg_node * args,const char * arg_name){
	arg_node * tmp = args;

	while(tmp!=NULL){
		if(strcmp(tmp->name,arg_name)==0)
			return tmp;
		tmp = tmp->next;
	}

	return NULL;
}

void scope_set_active(symbol_table ** st,unsigned int scope,char active){
	scope_entry * sc_temp = (*st)->scope_list;
	st_entry * st_temp, * ht_temp;
	unsigned int key;

	while(sc_temp!=NULL && sc_temp->scope!=scope)
		sc_temp = sc_temp->next;

	if(sc_temp!=NULL){
		st_temp = sc_temp->symbols;

		while(st_temp!=NULL){
			st_temp->active = active;

			// For the hash table
			key = generate_key(st_temp->name);
			ht_temp = (*st)->hash_table[key];
			while(ht_temp!=NULL){
				ht_temp->active = active;
				ht_temp = ht_temp->next;
			}

			st_temp = st_temp->next;
		}
	}
}
 
unsigned int count_func_args(st_entry * s){
	arg_node * arg = s->value_type.funVal->arguments;
	unsigned int total_arguments = 0;
	while(arg!=NULL){
		total_arguments++;
		arg = arg->next;
	}
	return(total_arguments);
}

unsigned int count_func_locals(symbol_table * st,char * funcname){
	unsigned int locals = 0;
	scope_entry * sc_e = st->scope_list;
	st_entry * st_e; 
 	while(sc_e){
 		st_e = sc_e->symbols;
 		while(st_e){
 			if(st_e->value_type.varVal->used_in_func){
 				if(st_e->type!=LCAL && strcmp(st_e->value_type.varVal->used_in_func,funcname)==0)
 					locals++;
 			}
 			st_e = st_e->next;
 		}
 		sc_e = sc_e->next;
 	}
 	return locals;
}