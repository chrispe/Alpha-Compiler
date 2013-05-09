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
	quads[quad_id].label = label;
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

	for(i=0;i<curr_quad;i++){
		// For the instruction format : op tmp1 tmp2 tmp3 
		if((quads[i].op >= add && quads[i].op < uminus)|| quads[i].op==and || quads[i].op==or){
 
		}
		 printf("Quad  (line %d)  (label:%d) (name:%s) \n",quads[i].line,quads[i].label,quads[i].result->sym->name);
	}
 
	fclose(quads_output);  
}

expr *lvalue_expr(st_entry * sym){   
   
	// Initialization of an expression.
	expr * sym_expr;
   	sym_expr = (expr *)malloc(sizeof(expr));
   
   	// Setting up the attributes of the expression.
   	sym_expr->next = NULL;
   	sym_expr->sym = sym;

   	switch(sym->type){
   		case GLOBAL_VAR || VAR || LCAL : sym_expr->type = var_e; break;
   		case LIBFUNC : sym_expr->type = library_func_e; break;
   		case USERFUNC : sym_expr->type = program_func_e; break;
   		default : assert(0); break;
   	}

   	return sym_expr;
}

expr * new_expr(expr_t type){
	expr * e = (expr *)malloc(sizeof(expr));
	e->type = type;
	return e;
}

expr * new_expr_const_str(char *s){
	expr * e = new_expr(const_str_e);
	e->str_value = strdup(s);
	return e;
}

expr * emit_iftableitem(expr *e,symbol_table ** st,unsigned int line){
	if(e->type != table_item_e)
		return e;
	expr * result = new_expr(var_e);
	result->sym = new_temp_var(st,line);
	emit(table_get_elem,e,e->index,result,curr_quad,line);
	return result;
}

expr * new_member_item_expr(expr * lvalue,char * name,symbol_table ** st,unsigned int line){
	
	// We emit if the item is used as r-value
	lvalue = emit_iftableitem(lvalue,st,line);

	// We create the item expression
	expr * item = new_expr(table_item_e);
	item->sym = lvalue->sym;
	item->index = new_expr_const_str(name);
	return item;
}