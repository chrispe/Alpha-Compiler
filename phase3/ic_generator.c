#include "ic_generator.h"
#include <assert.h>

// The array of the quads.
quad * quads = NULL;

// The toal number of quads.
unsigned int quads_total = 0;

// The current index on the array of quads.
unsigned int curr_quad = 0;

/* An expression list for the elist */
expr * elist_expr = NULL;

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
	current_quad->op = op;
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
		if(quads[i].op==call){
			fprintf(quads_output,"%d:\tCALL %s\n",i,quads[i].result->sym->name);
		}
		else if(quads[i].op==func_start){
			fprintf(quads_output,"%d:\tFUNCSTART %s\n",i,quads[i].result->sym->name);
		}
		else if(quads[i].op==func_end){
			fprintf(quads_output,"%d:\tFUNCEND %s\n",i,quads[i].result->sym->name);
		}
		else if(quads[i].op==param){
			fprintf(quads_output,"%d:\tPARAM %s \n",i,quads[i].result->sym->name);
		}
		else if(quads[i].op==get_ret_val){
			fprintf(quads_output,"%d:\tGETRETVAL %s \n",i,quads[i].result->sym->name);
		}
		else if(quads[i].op==table_get_elem){
			fprintf(quads_output,"%d:\tTABLEGETELEM %s %s %s\n",i,quads[i].arg1->sym->name,quads[i].arg2->str_value, quads[i].result->sym->name);
		}
		else if(quads[i].op==table_set_elem){
			fprintf(quads_output,"%d:\tTABLESETELEM %s %s\n",i,quads[i].arg1->sym->name,quads[i].arg2->str_value);
		}
		else if(quads[i].op==assign){
			fprintf(quads_output,"%d:\tASSIGN %s %s\n",i,quads[i].result->sym->name,quads[i].arg1->sym->name);
		}

		 if(quads[i].result!=NULL && quads[i].result->sym!=NULL)
		 	printf("Quad  (line %d)  (label:%d) (name:%s) (type:%s) \n",quads[i].line,quads[i].label,quads[i].result->sym->name,opcode_to_str(quads[i].op));
		 else if(quads[i].arg1!=NULL && quads[i].arg1->sym!=NULL)
		 	printf("Quad  (line %d)  (label:%d) (name:%s) (type:%s) \n",quads[i].line,quads[i].label,quads[i].arg1->sym->name,opcode_to_str(quads[i].op));
		 else
		 	printf("Quad  (line %d)  (label:%d) (name:unknown symbol) (type:%s) \n",quads[i].line,quads[i].label,opcode_to_str(quads[i].op));
	}
 
	fclose(quads_output);  
}

char * opcode_to_str(opcode op){
 
	switch(op){
		case assign:	return("assign"); 
		case func_start: return("func_start");
		case func_end: return("func_end");
		case table_get_elem: return("table_get_elem"); 
		case table_set_elem: return("table_set_elem");  
		case get_ret_val: return("get_ret_val"); 
		case param: return("param"); 
		case call: return("call");
		default : return("wtf");
	}
}

expr *lvalue_expr(st_entry * sym){   
   
	// Initialization of an expression.
	expr * sym_expr;
   	sym_expr = (expr *)malloc(sizeof(expr));
   
   	// Setting up the attributes of the expression.
   	sym_expr->next = NULL;
   	sym_expr->sym = sym;

   	switch(sym->type){
   		case GLOBAL_VAR || VAR || LCAL || FORMAL || TEMP_VAR : sym_expr->type = var_e; break;
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

expr * make_call(expr * lvalue,expr * elist,symbol_table ** st,unsigned int line){

	// Some variables
	expr * func = emit_iftableitem(lvalue,st,line);
	expr * params = NULL;
	expr * temp;

	// We use params as a stack for the elist
	params = elist;
	elist = elist->next;
	params->next = NULL;
	 
	// We create the stack 
	while(elist){
		temp = elist->next;
		elist->next = params;
		params = elist;
		elist =temp;
	}
 	
 	// We emit each expression of the stack
 	while(params){
 		emit(param,NULL,NULL,params,curr_quad,line);
 		params = params->next;
 	}

	emit(call,NULL,NULL,func,curr_quad,line);
	expr * result = new_expr(var_e);
	result->sym = new_temp_var(st,line);
	emit(get_ret_val,NULL,NULL,result,curr_quad,line);
	return result;

}
 