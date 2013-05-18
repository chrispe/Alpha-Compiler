#include "ic_generator.h"
#include <assert.h>

// The array of the quads.
quad * quads = NULL;

// The toal number of quads.
unsigned int quads_total = 0;

// The current index on the array of quads.
unsigned int curr_quad = 0;

/* An expression list for the index items */
expr * index_expr = NULL;

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
		if(quads[i].op == uminus || quads[i].op == not){

			fprintf(quads_output,"%d:\t%s %s %s\n",i,opcode_to_str(quads[i].op),expr_to_str(quads[i].arg1),expr_to_str(quads[i].result));
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
			fprintf(quads_output,"%d:\tPARAM %s \n",i,expr_to_str(quads[i].result));
		}
		else if(quads[i].op==get_ret_val){
			fprintf(quads_output,"%d:\tGETRETVAL %s \n",i,quads[i].result->sym->name);
		}
		else if(quads[i].op==table_get_elem || quads[i].op==add || quads[i].op==sub){
			fprintf(quads_output,"%d:\t%s %s %s %s\n",i,opcode_to_str(quads[i].op),expr_to_str(quads[i].arg1),expr_to_str(quads[i].arg2), expr_to_str(quads[i].result));
		}
		else if(quads[i].op==table_set_elem){
			fprintf(quads_output,"%d:\tTABLESETELEM %s %s %s\n",i,expr_to_str(quads[i].arg1), expr_to_str(quads[i].arg2), expr_to_str(quads[i].result));
		}
		else if(quads[i].op==assign){
			fprintf(quads_output,"%d:\tASSIGN %s %s\n",i,expr_to_str(quads[i].arg1),expr_to_str(quads[i].result));
		}
		else if(quads[i].op==table_create){
			fprintf(quads_output,"%d:\tTABLECREATE %s\n",i,quads[i].result->sym->name);
		}
 		if(quads[i].result!=NULL && quads[i].result->sym!=NULL)
       		printf("Quad  (line %d)  (label:%d) (name:%s) (type:%s) \n",quads[i].line,quads[i].label,quads[i].result->sym->name,opcode_to_str(quads[i].op));
     	else if(quads[i].arg1!=NULL && quads[i].arg1->sym!=NULL)
       		printf("Quad  (line %d)  (label:%d) (name:%s) (type:%s) \n",quads[i].line,quads[i].label,quads[i].arg1->sym->name,opcode_to_str(quads[i].op));
      
	}
 
	fclose(quads_output);  
}

char * opcode_to_str(opcode op){
 
	switch(op){
		case assign:	return("ASSIGN"); 
		case func_start: return("FUNCSTART");
		case func_end: return("FUNCEND");
		case table_get_elem: return("TABLEGETELEM"); 
		case table_set_elem: return("TABLESETELEM");  
		case get_ret_val: return("GETRETVAL"); 
		case param: return("PARAM"); 
		case call: return("CALL");
		case uminus: return("UMINUS");
		case not: return("NOT");
		case add: return("ADD");
		case sub: return("SUB");
		default : assert(0);
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
   		default : break;
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

expr * new_expr_const_num(double num){
	expr * e = new_expr(const_num_e);
	e->num_value = num;
	return e;
}

expr * new_expr_const_int(int num){
	expr * e = new_expr(const_int_e);
	e->int_value = num;
	return e;
}

expr * new_expr_const_bool(unsigned int b){
	expr * e = malloc(sizeof(expr));
	e->type = const_bool_e;
	e->bool_value = b;
	return e;
}

char * expr_to_str(expr * e){
	char * temp = malloc(50);
	if(e==NULL)
		return "" ; 

	switch(e->type){
		case const_int_e: sprintf(temp, "%d", e->int_value); break;
		case const_num_e: sprintf(temp, "%lf", e->num_value);break;
		case const_str_e: return e->str_value;
		case nil_e: return "";
		case const_bool_e:{
			if(e->bool_value==1)
				return("TRUE");
			return("FALSE");
		}
		default: return(e->sym->name);
	}
	return temp;
}

expr * make_call(expr * lvalue,expr * elist,symbol_table ** st,unsigned int line){

	// Some variables
	expr * func = emit_iftableitem(lvalue,st,line);
	expr * params = NULL;
	expr * temp;

	// We use params as a stack for the elist
	params = elist;
	if(elist!=NULL)elist = elist->next;
	if(params!=NULL)params->next = NULL;
	 
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
	(*st)->last_symbol = result->sym;
	emit(get_ret_val,NULL,NULL,result,curr_quad,line);
	return result;
}

void check_uminus(expr * e,unsigned int line){
	if(	e->type == const_bool_e	 	||
		e->type == const_str_e   	||
		e->type == nil_e  		 	||
		e->type == new_table_e   	||
		e->type == program_func_e   ||
		e->type == library_func_e   ||
		e->type == bool_expr_e)
		comp_error("Illegal expression to unary minus.\n",line);
}

void comp_error(char * error,unsigned int line){
	printf("Compile error at line %d : %s\n",line,error);
}

unsigned int name_is_temp(char * n){
	return (*n == '$');
}

unsigned int expr_is_temp(expr * e){
	return (e->sym && e->sym->type == var_e && name_is_temp(e->sym->name));
}