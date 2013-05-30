%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
 
 	#include "tc_generator.h"
	#define YYPARSE_PARAM st
	
	int yyerror (const char * yaccProvideMessage);
	
	// Importing variables from yylex.
	extern int yylex(void);
	extern int yylineno;
	extern char * yytext;
	extern FILE * yyin;

	// A temporary symbol pointing to the last function
	st_entry * func_entry = NULL;

	// Some parameters for the elist
	method_call_param m_param;
 
 	// A temporary expression used to make some things easier.
 	expr * temp_expr = NULL;

 	// A stack for pushing a useful variable when entering a function
 	expr * expr_stack = NULL;

 	// Some variables used for the (for) statement
 	unsigned int for_test = 0;
 	unsigned int for_enter = 0;

 	// The lists in which we save the labels for patching.
 	list_node * break_list = NULL;
 	list_node * con_list = NULL;

 	stack_node * break_stack = NULL;
 	stack_node * con_stack = NULL;

 	// The index for the jump before a function declaration
 	unsigned int funcstart_jump;

 	// The stack for patching the jumps before the function declaration
 	st_entry * func_sym_temp = NULL;
 	str_stack_node * func_jump_decl_stack = NULL;

%}
%error-verbose
%start program
%defines 
%output="parser.c"

%union{
	int intval;
	double fltval;
	char * strval;
	struct SymbolTableEntry * symbol;
	struct expr_s * expression;
}

%token <intval> INTEGER;
%token <fltval> REAL;
%token <strval> STRING;
%token <strval> IDENTIFIER;

%token <strval>	IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND NOT OR LOCAL TRUE FALSE NIL
%token <strval> EQUAL PLUS MINUS MULTI SLASH PERCENT DEQUAL NEQUAL DPLUS DMINUS GREATER LESS EQ_GREATER EQ_LESS
%token <strval> BRACE_L BRACE_R BRACKET_L BRACKET_R PAREN_L PAREN_R SEMICOLON COMMA COLON DCOLON DOT DDOT

%type <strval> stmt  assignexpr const primary member call callsuffix normcall methodcall term   if_prefix else_prefix forprefix M N
%type <strval> elist objectdef funcdef indexedelem indexed idlist block ifstmt block_in whilestmt forstmt func_temp whilesecond whilestart temp_indexed
%type <expression> expr lvalue con_elist
 
 
%left EQUAL
%left OR
%left AND
%nonassoc DEQUAL NEQUAL
%nonassoc GREATER EQ_GREATER LESS EQ_LESS
%left PLUS MINUS
%left MULTI SLASH PERCENT
%right NOT UMINUS DPLUS DMINUS
%left DOT DDOT
%left BRACKET_R BRACKET_L
%left PAREN_R PAREN_L
%nonassoc IF_TERM
%nonassoc ELSE
%% 

program:
		program stmt 
		| {}
		;

stmt:
		expr SEMICOLON { fun_rec=0; reset_tmp_var_counter(); }
		| BREAK SEMICOLON {
			if(scope_loop<=0)
				yyerror("Cannot use break; outside of a loop.");
			else{
				break_list = stack_top(break_stack);
				break_list = list_insert(break_list,curr_quad);
				break_stack->head = break_list;
				emit(jump,NULL,NULL,NULL,-1,yylineno);
			}
		}
		| CONTINUE SEMICOLON {
			if(scope_loop<=0)
				yyerror("Cannot use continue; outside of a loop.");
			else{
							
				con_list = stack_top(con_stack);
				con_list = list_insert(con_list,curr_quad);
				con_stack->head = con_list;
				emit(jump,NULL,NULL,NULL,-1,yylineno);
			}
		}
		| forstmt {}
		| whilestmt {}
		| block {}
		| ifstmt {}
		| funcdef {}
		| returnstmt {}
		| SEMICOLON {}
		;

expr:
		assignexpr {$<expression>$ = $<expression>1;}
		|	expr PLUS expr 	{
				$<expression>$ = emit_arithm((symbol_table **)st,add,$<expression>1,$<expression>3,$<expression>$,0,yylineno);
				temp_expr = $<expression>$;
		}
		|	expr MINUS expr	{
				$<expression>$ = emit_arithm((symbol_table **)st,sub,$<expression>1,$<expression>3,$<expression>$,0,yylineno);
				temp_expr = $<expression>$;
		}
		|	expr MULTI expr {
				$<expression>$ = emit_arithm((symbol_table **)st,mul,$<expression>1,$<expression>3,$<expression>$,0,yylineno);
				temp_expr = $<expression>$;
		}
		|	expr SLASH expr {
				$<expression>$ = emit_arithm((symbol_table **)st,op_div,$<expression>1,$<expression>3,$<expression>$,0,yylineno);
				temp_expr = $<expression>$;
			}
		|	expr PERCENT expr {
				$<expression>$ = emit_arithm((symbol_table **)st,mod,$<expression>1,$<expression>3,$<expression>$,0,yylineno);
				temp_expr = $<expression>$;
			}
		|	expr GREATER expr {
				$<expression>$ = emit_relop((symbol_table **)st,if_greater,$<expression>1,$<expression>3,$<expression>$,0,yylineno);
				temp_expr = $<expression>$;
		}
		|	expr EQ_GREATER expr {
				$<expression>$ = emit_relop((symbol_table **)st,if_greq,$<expression>1,$<expression>3,$<expression>$,0,yylineno);
				temp_expr = $<expression>$;
		}
		|	expr LESS expr {
				$<expression>$ = emit_relop((symbol_table **)st,if_less,$<expression>1,$<expression>3,$<expression>$,0,yylineno);
				temp_expr = $<expression>$;
		}
		|   expr EQ_LESS expr {
				$<expression>$ = emit_relop((symbol_table **)st,if_leq,$<expression>1,$<expression>3,$<expression>$,0,yylineno);
				temp_expr = $<expression>$;
		}
		|	expr DEQUAL expr {
				$<expression>$ = emit_relop((symbol_table **)st,if_eq,$<expression>1,$<expression>3,$<expression>$,0,yylineno);
				temp_expr = $<expression>$;
		}
		|	expr NEQUAL expr {
				$<expression>$ = emit_relop((symbol_table **)st,if_neq,$<expression>1,$<expression>3,$<expression>$,0,yylineno);
				temp_expr = $<expression>$;
		}
		|	expr AND expr {
				$<expression>$ = new_expr(bool_expr_e);
				$<expression>$->sym = new_temp_var(st,yylineno);
				emit(and,$<expression>1,$<expression>3,$<expression>$,0,yylineno);
				temp_expr = $<expression>$;
		}
		|	expr OR expr {
				$<expression>$ = new_expr(bool_expr_e);
				$<expression>$->sym = new_temp_var(st,yylineno);
				emit(or,$<expression>1,$<expression>3,$<expression>$,0,yylineno);
				temp_expr = $<expression>$;
		}
		| 	term {$<expression>$ = $<expression>1;}
		;

term:
		PAREN_L expr PAREN_R	{$<expression>$ = $<expression>2; temp_expr=$<expression>$;}
		| MINUS expr %prec UMINUS {
			check_uminus($<expression>2,yylineno);
			if(is_num_expr($<expression>2)){
				$<expression>$ = emit_arithm((symbol_table **)st,mul,$<expression>2,new_expr_const_int(-1),$<expression>$,-1,yylineno);
			}
			else{
				$<expression>$ = new_expr(arithm_expr_e);
				$<expression>$->sym =  new_temp_var(st,yylineno);
				emit(uminus,$<expression>2,NULL,$<expression>$,-1,yylineno);
			}
			temp_expr = $<expression>$;
		}
		| NOT expr {
			$<expression>$ = new_expr(bool_expr_e);
			$<expression>$->sym = new_temp_var(st,yylineno);
			temp_expr = $<expression>$;
			emit(not,$<expression>2,NULL,$<expression>$,-1,yylineno);
		}
		| lvalue DPLUS {
			if(fun_rec)printf("Error at line %d : %s is a function, cannot assign to a function.\n",yylineno,$$);
			else{
				$<expression>$ = new_expr(var_e);
				$<expression>$->sym = new_temp_var(st,yylineno);
				if($<expression>1->type==table_item_e){
					expr * value = emit_iftableitem($<expression>1,st,yylineno);
					emit(assign,value,NULL,$<expression>$,-1,yylineno);
					emit(add,value,new_expr_const_int(1),value,-1,yylineno);
					emit(table_set_elem,$<expression>1,$<expression>1->index,value,-1,yylineno);
				}
				else{
					emit(assign,$<expression>1,NULL,$<expression>$,-1,yylineno);
					emit(add,$<expression>1,new_expr_const_int(1),$<expression>1,-1,yylineno);
				}
				temp_expr = $<expression>$;
			}
		}
		| DPLUS lvalue {
			if(fun_rec)printf("Error at line %d : %s is a function, cannot assign to a function.\n",yylineno,$$);
			else {
				if($<expression>2->type == table_item_e){
					$<expression>$ = emit_iftableitem($<expression>2,st,yylineno);
					emit(add,$<expression>$,new_expr_const_int(1),$<expression>$,-1,yylineno);
					emit(table_set_elem,$<expression>2,$<expression>2->index,$<expression>$,-1,yylineno);
				}
				else{
					emit(add,$<expression>2,new_expr_const_int(1),$<expression>2,-1,yylineno);
					$<expression>$ = new_expr(arithm_expr_e);
					$<expression>$->sym = new_temp_var(st,yylineno);
					emit(assign,$<expression>2,NULL,$<expression>$,-1,yylineno);
				}
				temp_expr = $<expression>$;
			}
		}
		| lvalue DMINUS {
			if(fun_rec)printf("Error at line %d : %s is a function, cannot assign to a function.\n",yylineno,$$);
			else{
				$<expression>$ = new_expr(var_e);
				$<expression>$->sym = new_temp_var(st,yylineno);
				if($<expression>1->type==table_item_e){
					expr * value = emit_iftableitem($<expression>1,st,yylineno);
					emit(assign,value,NULL,$<expression>$,-1,yylineno);
					emit(sub,value,new_expr_const_int(1),value,-1,yylineno);
					emit(table_set_elem,$<expression>1,$<expression>1->index,value,-1,yylineno);
				}
				else{
					emit(assign,$<expression>1,NULL,$<expression>$,-1,yylineno);
					emit(sub,$<expression>1,new_expr_const_int(1),$<expression>1,-1,yylineno);
				}
			}
			temp_expr = $<expression>$;
		}
		| DMINUS lvalue {
			if(fun_rec)printf("Error at line %d : %s is a function, cannot assign to a function.\n",yylineno,$$);
			else {
				if($<expression>2->type == table_item_e){
					$<expression>$ = emit_iftableitem($<expression>2,st,yylineno);
					emit(sub,$<expression>$,new_expr_const_int(1),$<expression>$,-1,yylineno);
					emit(table_set_elem,$<expression>2,$<expression>2->index,$<expression>$,-1,yylineno);
				}
				else{
					emit(sub,$<expression>2,new_expr_const_int(1),$<expression>2,-1,yylineno);
					$<expression>$ = new_expr(arithm_expr_e);
					$<expression>$->sym = new_temp_var(st,yylineno);
					emit(assign,$<expression>2,NULL,$<expression>$,-1,yylineno);
				}
				temp_expr = $<expression>$;
			}
		}
		| primary {$<expression>$ = $<expression>1; }
		;

primary:
		lvalue	{
			$<expression>$ = emit_iftableitem($1,st,yylineno);
			temp_expr = $<expression>$;
		}
		| const { 
			$<expression>$ = $<expression>1;
			temp_expr = $<expression>$;
		}
		| call  { 
			$<expression>$ = $<expression>1; 
			temp_expr = $<expression>$;}
		| objectdef {

			$<expression>$ = new_expr(new_table_e);
			$<expression>$ = $<expression>1;
			temp_expr = $<expression>$;
		}
		| PAREN_L funcdef PAREN_R {
	 		
			$<expression>$ = new_expr(program_func_e);
			($<expression>$)->sym = $<symbol>2;
			temp_expr = $<expression>$;

		}
		;

const:
		REAL {$<expression>$=new_expr_const_num(yylval.fltval);temp_expr = $<expression>$;}
		| INTEGER {$<expression>$=new_expr_const_int(yylval.intval);temp_expr = $<expression>$;}
		| STRING {$<expression>$ = new_expr_const_str(yylval.strval);temp_expr = $<expression>$;}
		| NIL {$<expression>$ = new_expr(nil_e);temp_expr = $<expression>$;}
		| TRUE {$<expression>$ = new_expr_const_bool(1);temp_expr = $<expression>$;}
		| FALSE {$<expression>$ = new_expr_const_bool(0);temp_expr = $<expression>$;}
		;

assignexpr:
		lvalue EQUAL{expr_started=1;} 
		expr { 
			expr_started=0; 
			if(fun_rec)
				printf("Error at line %d: '%s' is a declared function, cannot assign to a function.\n",yylineno,$2);
			fun_rec=0;

			// Careful with the labels
			// (had tempexpr on exp4)
			if(($1)->type==table_item_e){
				emit(table_set_elem,$<expression>1,$<expression>1->index,$<expression>4,-1,yylineno);
				$<expression>$ = emit_iftableitem($<expression>1,st,yylineno);
				$<expression>$->type=assign_expr_e;
			}
			else{
				emit(assign,$<expression>4,NULL,$<expression>1,-1,yylineno);
				$<expression>$ = new_expr(assign_expr_e);
				 
				if($<expression>1->sym->type!=TEMP_VAR){
					($<expression>$)->sym = new_temp_var(st,yylineno);
					emit(assign,$<expression>1,NULL,$<expression>$,-1,yylineno);
				}
				else $<expression>$->sym = $<expression>1->sym;
				
			}
		}
		;

lvalue:
		IDENTIFIER {

			// Adding the id to the symbol table.
			// Every required checking is included in the following function.
			add_variable((symbol_table **)st, $1,yylineno);
			$<expression>$ = lvalue_expr((*((symbol_table **)st))->last_symbol);

		}
		| LOCAL IDENTIFIER {

			// Adding the local id to the symbol table.
			// Every required checking is included in the following function.
			add_local_variable((symbol_table **)st, $2,yylineno);
			$<expression>$ = lvalue_expr((*((symbol_table **)st))->last_symbol);
		}
		| DCOLON IDENTIFIER {

			// We check that the global variable exists
			// The whole proccess is handled by the following funciton.
			check_global_variable((symbol_table **)st, $2,yylineno);
			$<expression>$ = lvalue_expr((*((symbol_table **)st))->last_symbol);
		}
		| member {$<expression>$ = $<expression>1;}
		;

member:
		lvalue DOT IDENTIFIER {
			$<expression>$ = new_member_item_expr($1,$3,st,yylineno);
		}
		| lvalue BRACKET_L expr BRACKET_R {
			$1 = emit_iftableitem($1,st,yylineno);
			$<expression>$ = new_expr(table_item_e);
			($<expression>$)->sym = ($1)->sym;
			($<expression>$)->index = $3;
		}
		| call DOT IDENTIFIER {}
		| call BRACKET_L expr BRACKET_R {}
		;

call:
		call PAREN_L elist PAREN_R {
			$<expression>$ = make_call($<expression>1,m_param.elist,(symbol_table **)st,yylineno);
			m_param.elist = NULL;
		}
		| lvalue callsuffix {
			if(m_param.method){
				expr * self = $1;
				$1 = emit_iftableitem(new_member_item_expr(self,m_param.name,st,yylineno),st,yylineno);
				self->next = m_param.elist;
				m_param.elist = self;
			}
			$<expression>$ = make_call($1,m_param.elist,(symbol_table **)st,yylineno);
			m_param.elist = NULL;
		}
		| PAREN_L funcdef PAREN_R PAREN_L elist PAREN_R {
			expr * func = new_expr(program_func_e);
			func->sym = $<symbol>2;
			$<expression>$ = make_call(func,m_param.elist,(symbol_table **)st,yylineno);
		}
		;

callsuffix:
		normcall {
			$$ = $1;
		}
		| methodcall {
			$$ = $1;
		}
		;

normcall:
		PAREN_L elist PAREN_R {
			m_param.method = 0;
		}
		;

methodcall:
		DDOT IDENTIFIER PAREN_L elist PAREN_R {
			m_param.elist = $<expression>4;
			m_param.method = 1;
			m_param.name = malloc(strlen($2)+1);
			strcpy(m_param.name,$2);
		}
		;

objectdef:
		BRACKET_L elist BRACKET_R {
			int i=0;
			expr * table = new_expr(new_table_e);
			table->sym = new_temp_var(st,yylineno);
			emit(table_create,NULL,NULL,table,curr_quad,yylineno);

			expr * temp = m_param.elist;
			while(temp){
				emit(table_set_elem,table,new_expr_const_int(i),temp,-1,yylineno);
				temp = temp->next;
				i++;
			}
			$<expression>$ = table;
		}
		| BRACKET_L indexed BRACKET_R {
	 
			expr * table = new_expr(new_table_e);
			expr * temp = $<expression>2;
			table->sym = new_temp_var(st,yylineno);
			emit(table_create,NULL,NULL,table,-1,yylineno);

			while(temp){
				emit(table_set_elem,table,temp,temp->index,-1,yylineno);
				temp = temp->next;
			}
			$<expression>$ = table;
			index_expr = NULL;
		 	
		}
		;

indexed:
		indexedelem temp_indexed{
			$<expression>$ = $<expression>1;
			$<expression>$->next = $<expression>2;		
		}
		;

temp_indexed: 
			COMMA indexedelem temp_indexed {			
				$<expression>$ = $<expression>2;
				$<expression>$->next = $<expression>3; }
			|	{$<expression>$ = NULL;}
			;


indexedelem:
			BRACE_L expr COLON expr BRACE_R	{
		 
				$<expression>$ = $<expression>2;
				$<expression>$->index = $<expression>4;
			}
			;

elist:	expr con_elist{
			$<expression>$ = $<expression>1;
			$<expression>$->next = $<expression>2;
			m_param.elist = $<expression>$;
		} 
		| {$<expression>$ = NULL; m_param.elist = NULL;}
		;

con_elist: COMMA expr con_elist	{
			$<expression>$ = $<expression>2;
			$<expression>$->next = $<expression>3;
			m_param.elist = $<expression>$;
		} 
		|	{$<expression>$ = NULL; m_param.elist = NULL;}
		;
 
func_temp:
		IDENTIFIER{
			// Adding the function(with name) to the symbol table.
			// Every required checking is included in the following method.
			add_function((symbol_table **)st,$1,yylineno,1);
			func_entry = (*((symbol_table **)st))->last_symbol;

			// We add funcstart quad
			st_entry * se = st_lookup_scope(*((symbol_table **)st),$1,scope_main);
			
			temp_expr = lvalue_expr(se);
			temp_expr->next = expr_stack;
			expr_stack = temp_expr;
			temp_expr = NULL;
			funcstart_jump = curr_quad;
			push_value(&func_jump_decl_stack,funcstart_jump);
			emit(jump,NULL,NULL,NULL,-1,yylineno);
			emit(func_start,NULL,NULL, lvalue_expr(se), curr_quad,yylineno);
			 
		} 
		PAREN_L {
			scope_main++;
			in_func=1;
			func_started=1;
			func_scope++;
		 	enter_scope_space();
		} 
		idlist PAREN_R{enter_scope_space();}  block { 
			func_scope--;
			in_func=0;
			
			// We add funcend quad
			st_entry * se = st_lookup_scope(*((symbol_table **)st),top(func_names),scope_main);
			emit(func_end,NULL,NULL, lvalue_expr(se), curr_quad,yylineno);
			patch_label(top_value(func_jump_decl_stack),curr_quad);
			pop(&func_jump_decl_stack);
			pop(&func_names);
			temp_expr = expr_stack;
			expr_stack = expr_stack->next;
			func_sym_temp = se;

		}
		| PAREN_L{
 			// Adding the function(without name) to the symbol table.
			// Every required checking is included in the following method.
			add_function((symbol_table **)st,NULL,yylineno,0);
			temp_expr = NULL;
			
			// We add funcstart quad
			st_entry * se = (*(symbol_table **)(st))->last_symbol;
			 
			temp_expr = lvalue_expr(se);
			temp_expr->next = expr_stack;
			expr_stack = temp_expr;
			temp_expr = NULL;
			funcstart_jump = curr_quad;
			push_value(&func_jump_decl_stack,funcstart_jump);
			emit(jump,NULL,NULL,NULL,-1,yylineno);
			emit(func_start,NULL,NULL, lvalue_expr(se), curr_quad,yylineno);
 		 	enter_scope_space();
 		  

		} idlist PAREN_R{enter_scope_space();} block {   
			func_var=0;
			func_scope--;
			in_func=0;
			st_entry * se = st_lookup_scope(*((symbol_table **)st),top(func_names),scope_main);
			 
			emit(func_end,NULL,NULL, lvalue_expr(se), curr_quad,yylineno);
			patch_label(top_value(func_jump_decl_stack),curr_quad);
			pop(&func_jump_decl_stack);
			pop(&func_names);
			temp_expr = expr_stack;
			expr_stack = expr_stack->next;
			func_sym_temp = se;
 
		}
		; 

funcdef:
		FUNCTION{
			// Starting function
			func_var=1;

			// We push the loop scope and the offset to a stack
			push_value(&loop_stack,scope_loop);
			push_value(&scope_offset_stack,get_current_scope_offset());

			// We set the new scope offset and loop scope to zero
			reset_curr_scope_offset();
			scope_loop=0;
		} 
		func_temp {
			// Function definition ended

			// We set the scope loop to the previous value
			scope_loop = top_value(loop_stack);
			 
			pop(&loop_stack);
			 
			// We decrease the scope space by two. 
			exit_scope_space();
			exit_scope_space();

			// We set the scope offset to the previous scope
			set_curr_scope_offset(top_value(scope_offset_stack));
			pop(&scope_offset_stack);

			// We set the symbol of this expression (poitning to the function symbol)
			$<symbol>$ = func_sym_temp;
		}
		;

idlist:
		IDENTIFIER {
 
 			// Adding the argument of a function to the symbol table.
			// Every required checking is included in the following method.
 			add_function_argument((symbol_table **)st,$1,yylineno,0);
 			increase_curr_scope_offset();
		}
		| idlist COMMA IDENTIFIER {

			// Adding the argument of a function to the symbol table.
			// Every required checking is included in the following method.
			add_function_argument((symbol_table **)st,$3,yylineno,1);
			increase_curr_scope_offset();
		}
		| {}
		;

block:
		BRACE_L {

			if(!func_started)
				scope_main++;
			else
				func_started = 0;

		} block_in BRACE_R {

			// We disable the local variables of the current scope
			scope_set_active((symbol_table **)st,scope_main,0);
			scope_main--;

		}
		;
 
block_in:	
		block_in stmt {}
		| {}
		;

ifstmt:
		if_prefix stmt %prec IF_TERM { 
			patch_label($<intval>1,curr_quad);


		}
		| if_prefix stmt else_prefix stmt { 
			patch_label($<intval>1,$<intval>3+1);
			patch_label($<intval>3,curr_quad);
		}
		;

else_prefix:
		ELSE {
			$<intval>$ = curr_quad;
			emit(jump,NULL,NULL,NULL,-1,yylineno);
		}
		;

if_prefix:
		IF PAREN_L expr PAREN_R{
			emit(if_eq,$<expression>3,new_expr_const_bool(1),new_expr_const_int(curr_quad+2),curr_quad+2,yylineno);
			$<intval>$ = curr_quad;
			emit(jump,NULL,NULL,NULL,-1,yylineno);
		}
		;

whilestmt:
		whilestart whilesecond
		stmt {
			scope_loop--;
			emit(jump,NULL,NULL,new_expr_const_int($<intval>1),$<intval>1,yylineno);
			patch_label($<intval>2,curr_quad);

			break_list = stack_top(break_stack);
			con_list = stack_top(con_stack);

			while(break_list){
				patch_label(break_list->value,curr_quad);
				break_list = break_list->next;
			}

			while(con_list){
				patch_label(con_list->value,$<intval>1);
				con_list = con_list->next;
			}
			
			break_stack = pop_node(break_stack);
			con_stack = pop_node(con_stack);
		} 
		;

whilestart:
		WHILE{
			scope_loop++;
			$<intval>$ = curr_quad;
 			break_stack = push_node(break_stack,NULL);
			con_stack = push_node(con_stack,NULL);
 
		}

whilesecond:
		PAREN_L expr PAREN_R {
			emit(if_eq,$<expression>2,new_expr_const_bool(1),new_expr_const_int(curr_quad+2),-1,yylineno);
			$<intval>$ = curr_quad;
			emit(jump,NULL,NULL,NULL,curr_quad,yylineno);
		}
		;


N: {
	$<intval>$ = curr_quad;
	emit(jump,NULL,NULL,NULL,-1,yylineno);
}

M: {
	$<intval>$ = curr_quad;
}

forprefix:
		FOR PAREN_L elist SEMICOLON M expr SEMICOLON{
			scope_loop++;
			$<intval>$ = $<intval>5;
			for_enter = curr_quad;
			emit(if_eq,$<expression>6,new_expr_const_bool(1),NULL,curr_quad,yylineno);
			break_stack = push_node(break_stack,NULL);
			con_stack = push_node(con_stack,NULL);
		}
		;

forstmt:
		forprefix N elist PAREN_R N stmt N {
			scope_loop--;
			patch_label(for_enter,$<intval>5+1);
			patch_label($<intval>2,curr_quad);
			patch_label($<intval>5,$<intval>1);
			patch_label($<intval>7,$<intval>2+1);
			patch_label($<intval>2-1,$<intval>5+1);

			break_list = stack_top(break_stack);
			con_list = stack_top(con_stack);

			while(break_list){
				patch_label(break_list->value,curr_quad);
				break_list = break_list->next;
			}

			while(con_list){
				patch_label(con_list->value,$<intval>2+1);
				con_list = con_list->next;
			}

			break_stack = pop_node(break_stack);
			con_stack = pop_node(con_stack);
		}
		;

returnstmt:
		RETURN SEMICOLON {
			if(func_scope==0)yyerror("Cannot use return; when not in a function.");
			else emit(ret,NULL,NULL,NULL,curr_quad,yylineno);
			
		
		}
		| RETURN expr SEMICOLON {
			if(func_scope==0)yyerror("Cannot use return; when not in a function.");
			else emit(ret,NULL,NULL,$<expression>2,curr_quad,yylineno);
			
		}
		;

%%

int yyerror (const char * yaccProvideMessage){
	fprintf(stderr,"Syntax error at line %d: %s\n",yylineno,yaccProvideMessage);
}

int main(int argc,char ** argv)
{
	m_param.elist = NULL;
 	symbol_table * st = NULL;
 	st = create_symbol_table();
 
    if (argc > 1) {
        if ((yyin = fopen(argv[1], "r")) == NULL) {
            	fprintf(stderr, "Cannot read file %s\n", argv[1]);
            	return 1;
        }
    }
    else
        yyin = stdin;

    printf("Compilation started...\n");
    printf("Generating intermediate code...");
	yyparse(&st);
	printf(" (DONE)\n");
	write_quads();
	printf("Generating target code...");
	generate_instructions();
	printf(" (DONE)\n");
	printf("Compilation has been completed.\n");
	print_instructions();
	 
	return 0;	
}