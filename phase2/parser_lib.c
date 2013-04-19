#include "parser_lib.h"

// This is the main scope.
unsigned int scope_main = 0;

// This is a scope for how deep the loop is.
unsigned int scope_loop = 0;

// A boolean to know if we're in a function.
char in_func = 0;
unsigned int func_scope = 0;

// A number which indicates how many function we have set by the prefix '$_(id)'
unsigned int func_signed = 0;

// A boolean which indicates if a function name has been called.
char func_started = 0;

// A temporary string for a lot of uses.
char * temp_str;

// A variable which indicates if we recognized a function symbol.
char fun_rec = 0;

// A variable which indicates if we reading the parameters of a function.
char func_var = 0;

// A temporary pointer to the argument stack of a function.
arg_node * arg_tmp = NULL;

char expr_started = 0;

// A stack which we push every time we visit a function
// and pop each time we leave a function.
str_stack_node * func_names = NULL;

void push(str_stack_node ** top,const char * newString){
	str_stack_node * newNode = malloc(sizeof(str_stack_node));
	newNode->str = malloc(strlen(newString)+1);
	strcpy(newNode->str,newString);
	newNode->next = *top;
	*top = newNode;
}

void pop(str_stack_node ** top){
	str_stack_node * temp;
	temp = *top;
	if(*top!=NULL){
		*top = (*top)->next;
		free(temp);
	}
}

char * top(str_stack_node * top){
	if(top!=NULL)return top->str;
	return NULL;
}

double modulo(double a, double b){
	int result = (int)(a/b);
	return a - (double)result * b;
}


void add_variable(symbol_table ** st, char * variable,unsigned int yylineno){
	 			int i;
 			st_entry * se = NULL;

 			// Lookup symbol table starting at the current 
 			// scope and going one level down in each loop
 			// until we get to know if the symbol exists.

			for(i=scope_main;i>=0;i--){
				se = st_lookup_scope(*((symbol_table **)st),variable,i);
				if(se!=NULL)break;
			}

			// If the symbol was found then we need to detect if we have access to it.
			if(se!=NULL){

				if(se->type!=USERFUNC && se->type!=LIBFUNC){
					//In case we are in a function and we try to access a non-global variable
					if(in_func && se->scope!=0 && (se->value_type.varVal->used_in_func==NULL ||
						strcmp(se->value_type.varVal->used_in_func,top(func_names))!=0))
						printf("Error at line %d: Variable '%s' not accessible.\n",yylineno,variable);
					else 
						printf("Variable '%s' was detected and used.\n", variable);
				}
				else if(expr_started==0){fun_rec = 1;printf("Function recognized as lvalue\n");}
			}
			else {
				// Else if the symbol could not be found we insert it in the symbol table.
				se = create_symbol(variable,1,scope_main,yylineno,VAR);
				st_insert((symbol_table **)st,&se);
				printf("Added variable '%s' in the symbol table.\n",variable);
			}
}


void add_local_variable(symbol_table ** st, char * variable,unsigned int yylineno){
	st_entry * se  = NULL;

	// We perform a lookup in the current scope
	se = st_lookup_scope(*((symbol_table **)st),variable,scope_main);

	// If the symbol was found
	if(se!=NULL){
		// We need to check that there is no collusion with library function.
		// else we make a reference to that variable.
		if(se->type==LIBFUNC && scope_main!=0)
			printf("Error at line %d: '%s' is a library function, must not be shadowed.\n",yylineno,variable);
		else 
			printf("Local variable '%s' was detected and used.\n",variable);
	}
	else{

		// We need to make sure that there is no library function with that name.
		// So we need to lookup on the global scope.
		se = st_lookup_scope(*((symbol_table **)st),variable,0);

		if(se!=NULL && se->type==LIBFUNC)
			printf("Error at line %d: '%s' is a library function, must not be shadowed.\n",yylineno,variable);
		else{

			// If the symbol could not be detected we insert it to
			// the symbol table on the current scope.
			if(scope_main==0){
				se = create_symbol(variable,1,0,yylineno,GLOBAL_VAR);
				printf("Added global variable '%s' in the symbol table.\n",variable);
			}
			else{
				// If we are under a function then it is ok to set local 
				// else we set it as a simple variable
				if(in_func){
					se = create_symbol(variable,1,scope_main,yylineno,LCAL);
					se = set_var_func(se,top(func_names));
					printf("Added local variable '%s' in the symbol table.\n",variable);
				}
				else{
					se = create_symbol(variable,1,scope_main,yylineno,VAR);
					printf("Added variable '%s' in the symbol table.\n",variable);
				}
			}
			st_insert((symbol_table **)st,&se);
		}
	}
}	

void check_global_variable(symbol_table ** st, char * variable,unsigned int yylineno){
	st_entry * se = NULL;

	// We perform a lookup in the global scope
	se = st_lookup_scope(*((symbol_table **)st),variable,0);

	if(se==NULL)
		printf("Error at line %d: Global variable '%s' could not be detected.\n",yylineno,variable);
	else
		printf("Global variable '%s' was detected and used.\n",variable); 
}

void add_function(symbol_table ** st, char * function,unsigned int yylineno,const char has_name){
	st_entry * se = NULL;
	if(has_name){
		push(&func_names,function);

		// We check that there is no symbol using the same
		// name with this of the new function.
		se = st_lookup_scope(*((symbol_table **)st),function,scope_main);

		// If a symbol has been detected we show the error.
		if(se!=NULL){
			if(se->type==LIBFUNC)
				printf("Error at line %d: '%s' is a library function, must not be shadowed.\n",yylineno,function);
			else {
				printf("Error at line %d: '%s' has already been declared as a ",yylineno,function);
				if(se->type==USERFUNC)
					printf("function.\n");
				else
					printf("variable.\n");
			}
		}
		else{
			// else we add the new symbol to the symbol table.
			se = create_symbol(function,1,scope_main,yylineno,USERFUNC);
			st_insert((symbol_table **)st,&se);
		}
	}
	else{
		 // We increase the scope of the function
		func_scope++;

		// We set that we are in a function
		in_func=1;

		// ...and the function has just started.
		func_started=1;

		// We generate a function name for this function
		// and insert the symbol to the symbol table
		temp_str = generate_func_name(func_signed);
		push(&func_names,temp_str);
		se = create_symbol(temp_str,1,scope_main,yylineno,USERFUNC);
			
		st_insert((symbol_table **)st,&se);
		func_signed++;
		scope_main++;
	}
}

void add_function_argument(symbol_table ** st, char * argument,unsigned int yylineno,const char comma){
	printf("fun par:%s\n",argument);

 	st_entry * se = NULL;

 	// We check that there is no other variable using the same name on the same scope.
 	se = st_lookup_scope(*((symbol_table **)st),argument,scope_main);
 	if(se!=NULL)printf("Error at line %d: '%s' has already be declared.\n",yylineno,argument);
 	else
 	{
 		// We check that there is no library function shadowing.
 		se = st_lookup_scope(*((symbol_table **)st),argument,0);
 		if(se!=NULL && se->type==LIBFUNC)
 			printf("Error at line %d: '%s' is a library function, cannot be shadowed.\n",yylineno,argument);
 		else{
 			se = create_symbol(argument,1,scope_main,yylineno,FORMAL);
 			se = set_var_func(se,top(func_names));
 			st_insert((symbol_table **)st,&se);
 			if(comma)args_insert(&arg_tmp,argument);
 			se = st_lookup_scope(*((symbol_table **)st),top(func_names),scope_main-1);
 			args_insert(&(se->value_type.funVal->arguments),argument);
 		}
 	}
}
