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
	quads[quad_id].result = new_expr_const_int(label);
	quads[quad_id].label = label;
}

// TO DO 
void write_quads(void){
	int i;
	FILE * quads_output; 

	char * ops[] = {"ASSIGN","ADD","SUB","MUL","DIV","MOD","UMINUS","AND","OR","NOT"
					"IF_EQ","IF_NOTEQ","IF_LESSEQ","IF_GREATEREQ","IF_LESS","IF_GREATER"
					"CALL","PARAM","RET","GETRETVAL","FUNCSTART","FUNCEND"
					"JUMP","TABLE_CREATE","TABLEGETELEM","TABLESETITEM" };

	quads_output = fopen("quads.txt","w"); 
	if(quads_output==NULL)
		quads_output = stderr;  

	for(i=0;i<curr_quad;i++){
		// For the instruction format : op tmp1 tmp2 tmp3 
		if(quads[i].op == uminus || quads[i].op == not){

			fprintf(quads_output,"%d:\t%s %s %s\n",i,opcode_to_str(quads[i].op),expr_to_str(quads[i].arg1),expr_to_str(quads[i].result));
		}
		else if(quads[i].op==or || quads[i].op==and || (quads[i].op>=add && quads[i].op<=mod)){
			fprintf(quads_output,"%d:\t%s %s %s %s\n",i,opcode_to_str(quads[i].op),expr_to_str(quads[i].arg1),expr_to_str(quads[i].arg2),expr_to_str(quads[i].result));
		}
		else if(quads[i].op==call){
			fprintf(quads_output,"%d:\tCALL %s\n",i,quads[i].result->sym->name);
		}
		else if(quads[i].op==ret){
			fprintf(quads_output,"%d:\tRETURN %s\n",i,expr_to_str(quads[i].result));
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
		else if(quads[i].op==add || quads[i].op==sub){
			fprintf(quads_output,"%d:\t%s %s %s %s\n",i,opcode_to_str(quads[i].op),expr_to_str(quads[i].arg1),expr_to_str(quads[i].arg2), expr_to_str(quads[i].result));
		}
		else if(quads[i].op==table_get_elem){
			if(quads[i].arg2->type==const_str_e)
				fprintf(quads_output,"%d:\t%s %s '%s' %s\n",i,opcode_to_str(quads[i].op),expr_to_str(quads[i].arg1),expr_to_str(quads[i].arg2), expr_to_str(quads[i].result));
			else
				fprintf(quads_output,"%d:\t%s %s %s %s\n",i,opcode_to_str(quads[i].op),expr_to_str(quads[i].arg1),expr_to_str(quads[i].arg2), expr_to_str(quads[i].result));
		}
		else if(quads[i].op==table_set_elem){
			if(quads[i].arg2->type==const_str_e)
				fprintf(quads_output,"%d:\tTABLESETELEM %s '%s' %s\n",i,expr_to_str(quads[i].arg1), expr_to_str(quads[i].arg2), expr_to_str(quads[i].result));
			else
				fprintf(quads_output,"%d:\tTABLESETELEM %s %s %s\n",i,expr_to_str(quads[i].arg1), expr_to_str(quads[i].arg2), expr_to_str(quads[i].result));
		}
		else if(quads[i].op==assign){
			fprintf(quads_output,"%d:\tASSIGN %s %s\n",i,expr_to_str(quads[i].arg1),expr_to_str(quads[i].result));
		}
		else if(quads[i].op==table_create){
			fprintf(quads_output,"%d:\tTABLECREATE %s\n",i,quads[i].result->sym->name);
		}
		else if(quads[i].op==jump){
			fprintf(quads_output,"%d:\tJUMP %d\n",i,quads[i].result->int_value);
		}
		else if(quads[i].op>=if_eq && quads[i].op<=if_greater){
			fprintf(quads_output,"%d:\t%s %s %s %s\n",i,opcode_to_str(quads[i].op),expr_to_str(quads[i].arg1), expr_to_str(quads[i].arg2), expr_to_str(quads[i].result));
		}
 		//if(quads[i].result!=NULL && quads[i].result->sym!=NULL)
 
     	//else if(quads[i].arg1!=NULL && quads[i].arg1->sym!=NULL)
       	//	printf("Quad  (line %d)  (label:%d) (name:%s) (type:%s) \n",quads[i].line,quads[i].label,quads[i].arg1->sym->name,opcode_to_str(quads[i].op));
      
	}
 
	fclose(quads_output);  
}
 

char * opcode_to_str(opcode op){
	char * ops[] = {"ASSIGN","ADD","SUB","MUL","DIV","MOD","UMINUS","AND","OR","NOT",
					"IF_EQ","IF_NOTEQ","IF_LESSEQ","IF_GREATEREQ","IF_LESS","IF_GREATER",
					"CALL","PARAM","RET","GETRETVAL","FUNCSTART","FUNCEND",
					"JUMP","TABLE_CREATE","TABLEGETELEM","TABLESETITEM" };
	return(ops[op-assign]);
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
	emit(table_get_elem,e,e->index,result,-1,line);
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
		case const_str_e:{
			return e->str_value;
		}
		case nil_e: return "NIL";
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
	exit(0);
}

unsigned int name_is_temp(char * n){
	return (*n == '$');
}

unsigned int expr_is_temp(expr * e){
	return (e->sym && e->sym->type == var_e && name_is_temp(e->sym->name));
}
 
unsigned int arithm_expr_valid(expr * e){
	return( e->type == var_e
		|| e->type == arithm_expr_e
		|| e->type == const_num_e
		|| e->type == const_int_e
		|| e->type == table_item_e);
}

unsigned int is_num_expr(expr * e){
	return (e->type==const_int_e || e->type==const_num_e);
}

unsigned int is_num_double(expr * e){
	if(e->type==const_num_e)return 1;
	else return 0;
}

double get_expr_num_value(expr * e){
	if(e->type==const_num_e)
		return(e->num_value);
	else if(e->type==const_int_e)
		return((double)e->int_value);
	else assert(0);
}

expr * emit_arithm(symbol_table ** st,opcode op,expr * arg1,expr * arg2, expr * result, unsigned int label,unsigned int line){
	expr * e;
	unsigned int is_float = 0;
	double a_result;

	if(arithm_expr_valid(arg1) && arithm_expr_valid(arg2)){
		if(is_num_expr(arg1) && is_num_expr(arg2)){
			is_float = is_num_double(arg1) || is_num_double(arg2);
			double a_result = apply_arithm_op(op,get_expr_num_value(arg1),get_expr_num_value(arg2),line);
			if(is_float)
				result = new_expr_const_num(a_result);
			else
				result = new_expr_const_int((int)a_result);
		}
		else {
				result = new_expr(arithm_expr_e);
				result->sym = new_temp_var(st,line);
				emit(op,arg1,arg2,result,label,line);
		}
	}
	else 
		comp_error("Invalid expression type for arithmetic operation.",line);
	return result;
}

double apply_arithm_op(opcode op,double arg1,double arg2,unsigned int line){
	double result;
	switch(op){
		case add: result = arg1 + arg2; break;
		case sub: result = arg1 - arg2;	break;
		case mod: {
			if(arg2!=0)result = modulo(arg1,arg2);
			else{
				result = arg1;
				printf("\nWarning at line %d : Division by zero.\n",line);
			}
			break;
		}
		case op_div: {
			if(arg2!=0)result = arg1/arg2; 
			else{
				result = arg1;
				printf("\nWarning at line %d : Division by zero.\n",line);
			}
			break;
		}
		case mul: result = arg1 * arg2; break;
		default: assert(0);
	}
	return result;
}

expr * emit_relop(symbol_table ** st,opcode op,expr * arg1,expr * arg2, expr * result, unsigned int label,unsigned int line){
	expr * e = new_expr(bool_expr_e);
 	e->sym = new_temp_var(st,line);

 	if(is_num_expr(arg1) && is_num_expr(arg2)){
 		emit(assign,new_expr_const_bool(apply_boolean_op(op,arg1,arg2)),NULL,e,curr_quad,line);
 		return e;
 	}

	emit(op,arg1,arg2,new_expr_const_int(curr_quad+3),curr_quad,line);
	emit(assign,new_expr_const_bool(0),NULL,e,curr_quad,line);
	emit(jump,NULL,NULL,new_expr_const_int(curr_quad+2),curr_quad,line);
	emit(assign,new_expr_const_bool(1),NULL,e,curr_quad,line);
	return e;
}
 
unsigned int apply_boolean_op(opcode op,expr * arg1, expr *arg2){
	switch(op){
		case if_eq: return (get_expr_num_value(arg1) == get_expr_num_value(arg2));
		case if_neq: return (get_expr_num_value(arg1) != get_expr_num_value(arg2));
		case if_leq: return (get_expr_num_value(arg1) <= get_expr_num_value(arg2));
		case if_less: return (get_expr_num_value(arg1) < get_expr_num_value(arg2));
		case if_greq: return (get_expr_num_value(arg1) >= get_expr_num_value(arg2));
		case if_greater: return (get_expr_num_value(arg1) > get_expr_num_value(arg2));
		default: assert(0);
	}
} 

unsigned expr_to_boolean(expr * e){
	switch(e->type){
		case const_num_e: return(e->num_value!=0);
		case const_int_e: return(e->int_value!=0);
		case program_func_e: return 1;
		case library_func_e: return 1;
		case table_item_e: return 1;
		case nil_e: return 0;
		case const_str_e: return(e->str_value!="");
		case bool_expr_e: return(e->bool_value);
		default: assert(0);
	}
}

list_node * list_insert(list_node * head,unsigned int value){
	list_node * temp = head;

	list_node * new_node = malloc(sizeof(list_node));
	new_node->value = value;
	new_node->next = NULL;

	while(temp!=NULL && temp->next!=NULL)
		temp = temp->next;

	if(temp!=NULL)
		temp->next = new_node;
	else
		head = new_node;

	return head;
}

list_node * list_insert_two(list_node * head,unsigned int v1,unsigned int v2){
	list_node * temp = head;

	list_node * new_node = malloc(sizeof(list_node));
	new_node->value = v1;
	new_node->next_value = v2;
	new_node->next = NULL;

	while(temp!=NULL && temp->next!=NULL)
		temp = temp->next;

	if(temp!=NULL)
		temp->next = new_node;
	else
		head = new_node;

	return head;
}

list_node * merge_lists(list_node * list1, list_node * list2){
	list_node * temp = list1;

	if(list1==NULL)return list2;
	else if(list2==NULL)return list1;

	while(temp!=NULL && temp->next!=NULL)
		temp = temp->next;
	temp->next = list2;

	return list1;
}


stack_node * pop_node(stack_node *top){
	top = top->next;
	return top;
}

stack_node * push_node(stack_node * top, list_node * head){
	stack_node * newNode = malloc(sizeof(stack_node));
	newNode->head = head;
	newNode->next = top;
	return newNode;
}

list_node * stack_top(stack_node * top){
	if(top!=NULL)return (top->head);
	return NULL;
}