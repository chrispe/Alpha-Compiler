#include "ic_generator.h"
#include <assert.h>

// The array of the quads.
quad * quads = NULL;

// The toal number of quads.
unsigned int quads_total = 0;

// The current index on the array of quads.
unsigned int curr_quad = 0;

/* A number which indicates how many temporary
   variables we have set by the prefix '$v_(id)' */
unsigned int var_signed = 0;

/* Variables used to get the offset of each kind of symbol */
unsigned int program_var_offset = 0;
unsigned int func_local_offset = 0;
unsigned int formal_arg_offset = 0;

/* A counter for the scope space */
unsigned int scope_space_counter = 1;

void expand(void){
	assert(quads_total==curr_quad);
	
	// Allocating memory for new array
	quad * new_quad_arr = (quad *)malloc(NEW_SIZE);
	if(memerror(new_quad_arr,"new quad array"))exit(0);

	// Copying quads to the new array
	memcpy(new_quad_arr,quads,CURR_SIZE);

	quads = new_quad_arr;
	quads_total += EXPAND_SIZE;
}

void emit(opcode op,expr * arg1,expr * arg2, expr * result, unsigned int label,unsigned int line){
	
	// if the array index is on it's limit then expand
	if(curr_quad == quads_total)
		expand();

	// set the parameters of the new quad
	quad * current_quad = quads + curr_quad++;
	current_quad->arg1 = arg1;
	current_quad->arg2 = arg2;
	current_quad->result = result;
	current_quad->label = label;
	current_quad->line = line;
}

char * generate_temp_var_name(unsigned int id){

	char buffer[100];
	char * var_name;
	unsigned int len;

	// We first create a string of the id to count its length
	len = sprintf(buffer,"%d",id);

	// We create another string with fixed size
	var_name = malloc(len+4);

	// Give a value to the string
	sprintf (var_name, "$v_%d", id);

	return var_name;
}

void reset_temp_vars(void){
	var_signed = 0;
}

unsigned int get_current_scope(void){
	return scope_main;
}

// Stuff to be added here! 
st_entry * get_temp_var(symbol_table * st, unsigned int line){

	// We generate a name for the new temporary variable.
	char * var_name = generate_temp_var_name(var_signed);

	/* if the symbol using that name already exists then we
	   use this symbol else we create a new one.         */
	st_entry * symbol = st_lookup_scope(st,var_name,get_current_scope());
	if(symbol==NULL)
		symbol = create_symbol(var_name,1,scope_main,line,TEMP_VAR);

	symbol->space = get_current_scope_space();
	symbol->offset = get_current_scope_offset();

	var_signed++;

	return symbol;
}

scopespace_t get_current_scope_space(void){
	if(scope_space_counter == 1)
		return PROGRAM_VAR;
	else if(scope_space_counter % 2 == 0)
		return FORMAL_ARG;
	return FUNC_LOCAL;		
}

unsigned int get_current_scope_offset(void){
	switch(get_current_scope_space()) {
		case PROGRAM_VAR : return program_var_offset;
		case FUNC_LOCAL  : return func_local_offset;
		case FORMAL_ARG  : return formal_arg_offset;
		default: assert(0);
	}
}

void increase_curr_scope_offset(void){
	switch(get_current_scope_space()) {
		case PROGRAM_VAR : program_var_offset++; break;
		case FUNC_LOCAL  : func_local_offset++;  break;
		case FORMAL_ARG  : formal_arg_offset++;  break;
		default: assert(0);
	}
}

void enter_scope_space(void){
	scope_space_counter++;
}

void exit_scope_space(void){
	assert(scope_space_counter>1);
	scope_space_counter--;
}