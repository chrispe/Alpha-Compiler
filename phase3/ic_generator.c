#include "ic_generator.h"
#include <assert.h>

// The array of the quads.
quad * quads = NULL;

// The toal number of quads.
unsigned int quads_total = 0;

// The current index on the array of quads.
unsigned int curr_quad = 0;

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
 
void patch_label(unsigned int quad_id, unsigned int label){
	assert(quad_id<curr_quad);
	quads[quad_id].label = label
}

// TO DO 
void write_quads(void){
	int i;
	FILE * quads_output; 

	char * ops[] = {"assign","add","sub","mul","op_div","mod","uminus","and","or","not"
					"if_eq","if_neq","if_leq","if_greq","if_less","if_greater"
					"call","param","ret","get_ret_val","func_start","func_end"
					"jump","table_create","table_get_elem","table_set_elem" };

	quads_output = fopen("quads.txt","w"); 
	if(quads_output==NULL)
		quads_output = stderr;  

	for(i=0;i<quads_total;i++){
		if((quads[i].op >= add && quads[i].op < uminus)|| quads[i].op==and || quads[i].op==or){
 
		}
	}
 
	fclose(quads_output);  
}