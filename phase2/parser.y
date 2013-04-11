%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
 	#include "symbol_table.h"
	#define YYPARSE_PARAM st
	
	int yyerror (const char * yaccProvideMessage);
	
	// Importing variables from yylex.
	extern int yylex(void);
	extern int yylineno;
	extern char * yytext;
	extern FILE * yyin;

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

 	// A function which returns the modulo of two doubles,
 	// so as no floating point exception occures.
	double modulo(double a, double b){
		int result = (int)(a/b);
		return a - (double)result * b;
	}

	typedef struct str_stack_node{
		char * str;
		struct str_stack_node * next;
	}str_stack_node;

	// A stack which we push every time we visit a function
	// and pop each time we leave a function.
	str_stack_node * func_names = NULL;

	void push(str_stack_node ** top,const char * newString){
		str_stack_node * newNode = malloc(sizeof(str_stack_node));
		newNode->str = malloc(strlen(newString)+1);
		strcpy(newNode->str,newString);
		if(*top==NULL){
			newNode->next = NULL;
			*top = newNode;
		}
		else{
			newNode->next = *top;
			*top = newNode;
		}
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

%}
%error-verbose
%start program
%defines 
%output="parser.c"

%union{
	int intval;
	double fltval;
	char * strval;
}

%token <intval> INTEGER;
%token <fltval> REAL;
%token <strval> STRING;
%token <strval> IDENTIFIER;

%token <strval>	IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND NOT OR LOCAL TRUE FALSE NIL
%token <strval> EQUAL PLUS MINUS MULTI SLASH PERCENT DEQUAL NEQUAL DPLUS DMINUS GREATER LESS EQ_GREATER EQ_LESS
%token <strval> BRACE_L BRACE_R BRACKET_L BRACKET_R PAREN_L PAREN_R SEMICOLON COMMA COLON DCOLON DOT DDOT

%type <strval> stmt assignexpr lvalue const primary member call callsuffix normcall methodcall   term
%type <strval> elist objectdef indexedelem indexed funcdef idlist block ifstmt block_in whilestmt forstmt func_temp
%type <fltval> expr  
 
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
		| {printf("\n <--[Parsing Started]-->\n\n");}
		;

stmt:
		expr SEMICOLON {fun_rec=0;}
		| BREAK SEMICOLON {
			if(scope_loop>0)printf("break;\n");
			else yyerror("Cannot use break; outside of a loop.");
		}
		| CONTINUE SEMICOLON {
			if(scope_loop>0)printf("continue;\n");
			else yyerror("Cannot use continue; outside of a loop.");
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
		assignexpr {}
		|	expr PLUS expr 	{printf("<expr> + <expr>\n",$1,$3); $$ = $1 + $3;}
		|	expr MINUS expr	{printf("<expr> - <expr>\n",$1,$3); $$ = $1 - $3;}
		|	expr MULTI expr {printf("<expr> * <expr>\n",$1,$3); $$ = $1 * $3;}
		|	expr SLASH expr {
					printf("<expr> / <expr>\n",$1,$3);
					if($3!=0)$$ = $1/$3;
					else yyerror("Cannot divide by zero.");
			}
		|	expr PERCENT expr {

					printf("<expr> %% <expr>\n",$1,$3);
					if($3!=0)$$ = modulo($1,$3);
					else yyerror("Cannot divide by zero.");
			}
		|	expr GREATER expr {printf("<expr> > <expr>\n",$1,$3); $$ = ($1 > $3)?1:0;}
		|	expr EQ_GREATER expr {printf("<expr> >= <expr>\n",$1,$3); $$ = ($1 >= $3)?1:0;}
		|	expr LESS expr {printf("<expr> < <expr>\n",$1,$3); $$ = ($1 < $3)?1:0;}
		|   	expr EQ_LESS expr {printf("<expr> <= <expr>d\n",$1,$3);$$ = ($1 <= $3)?1:0;}
		|	expr DEQUAL expr {printf("<expr> == <expr>\n",$1,$3); $$ = ($1 == $3)?1:0;}
		|	expr NEQUAL expr {printf("<expr> != <expr>\n",$1,$3); $$ = ($1 != $3)?1:0;}
		|	expr AND expr {printf("<expr> and <expr>\n",$1,$3); $$ = ($1 && $3)?1:0;}
		|	expr OR expr {printf("<expr> or <expr>\n",$1,$3); $$ = ($1 || $3)?1:0;}
		| 	term {}
		;

term:
		PAREN_L expr PAREN_R	{printf("(expr)\n");}
		| MINUS expr %prec UMINUS {printf("-<expr>\n");}
		| NOT expr {printf("!<expr>\n");}
		| lvalue DPLUS {printf("lvalue++\n");}
		| DPLUS lvalue {printf("++lvalue\n");}
		| lvalue DMINUS {printf("lvalue--\n");}
		| DMINUS lvalue {printf("--lvalue\n");}
		| primary {}
		;

primary:
		lvalue	{}
		| const {}
		| call {}
		| objectdef {}
		| PAREN_L funcdef PAREN_R {}
		;

const:
		REAL {}
		| INTEGER {}
		| STRING {}
		| NIL {}
		| TRUE {}
		| FALSE {}
		;

assignexpr:
		lvalue EQUAL{} expr {if(fun_rec)printf("Error at line %d: '%s' is a declared function, cannot assign to a function.\n",yylineno,$$);}
		;

lvalue:
		IDENTIFIER {

 			int i;
 			st_entry * se = NULL;



 			// IF id is used as an argument
 			if(func_var == 1){
 				printf("Runned for %s\n",$$);
 				// We check that there is no other variable using the same name on the same scope.
 				se = st_lookup_scope(*((symbol_table **)st),$$,scope_main);
 				if(se!=NULL)printf("Error at line %d: '%s' has already be declared.\n",yylineno,$$);
 				else
 				{
 					// We check that there is no library function shadowing.
 					se = st_lookup_scope(*((symbol_table **)st),$$,0);
 					if(se!=NULL && se->type==LIBFUNC)printf("Error at line %d: '%s' is a library function, cannot be shadowed.\n",yylineno,$$);
 					else{
 						se = create_symbol($$,1,scope_main,yylineno,FORMAL);
 						se = set_var_func(se,top(func_names));
 						st_insert((symbol_table **)st,&se);
 					}
 				}
 			}
 			else
 			{
 				//ELSE :

 				// Lookup symbol table starting at the current 
 				// scope and going one level down in each loop
 				// until we get to know if the symbol exists.

				for(i=scope_main;i>=0;i--){
					se = st_lookup_scope(*((symbol_table **)st),$$,i);
					if(se!=NULL)break;
				}

				// If the symbol was found then we need to detect if we have access to it.
				if(se!=NULL){

					if(se->type!=USERFUNC && se->type!=LIBFUNC){
						//In case we are in a function and we try to access a non-global variable
						if(in_func && se->scope!=0 && (se->value_type.varVal->used_in_func==NULL ||
							strcmp(se->value_type.varVal->used_in_func,top(func_names))!=0))
							printf("Error at line %d: Variable '%s' not accessible.\n",yylineno,$$);
						else 
							printf("Variable '%s' was detected and used.\n", $$);
					}
					else fun_rec = 1;
				}
				else {
					// Else if the symbol could not be found we insert it in the symbol table.
					se = create_symbol($$,1,scope_main,yylineno,VAR);
					st_insert((symbol_table **)st,&se);
					printf("Added variable '%s' in the symbol table.\n",$$);
				}
			}
		}
		| LOCAL IDENTIFIER {
			st_entry * se  = NULL;

			// We perform a lookup in the current scope
			se = st_lookup_scope(*((symbol_table **)st),$2,scope_main);

			// If the symbol was found
			if(se!=NULL){
				// We need to check that there is no collusion with library function.
				// else we make a reference to that variable.
				if(se->type==LIBFUNC)
					printf("Error at line %d: '%s' is a library function, must not be shadowed.\n",yylineno,$2);
				else printf("Local variable '%s' was detected and used.\n",$2);
			}
			else{
				// If the symbol could not be detected we insert it to
				// the symbol table on the current scope.
				if(scope_main==0){
					se = create_symbol($2,1,0,yylineno,GLOBAL_VAR);
					printf("Added global variable '%s' in the symbol table.\n",$2);
				}
				else{
					// If we are under a function then it is ok to set local 
					// else we set it as a simple variable
					if(in_func){
						se = create_symbol($2,1,scope_main,yylineno,LCAL);
						se = set_var_func(se,top(func_names));
						printf("Added local variable '%s' in the symbol table.\n",$2);
					}
					else{
						se = create_symbol($2,1,scope_main,yylineno,VAR);
						printf("Added variable '%s' in the symbol table.\n",$2);
					}
				}
			
				st_insert((symbol_table **)st,&se);

			}

		}
		| DCOLON IDENTIFIER {
			st_entry * se = NULL;
			// We perform a lookup in the global scope
			se = st_lookup_scope(*((symbol_table **)st),$2,0);
			if(se==NULL)
				printf("Global variable '%s' could not be detected.\n",$2);
			else
				printf("Global variable '%s' was detected and used.\n",$2); 
		}
		| member {}
		;

member:
		lvalue DOT IDENTIFIER {}
		| lvalue BRACKET_L expr BRACKET_R {}
		| call DOT IDENTIFIER {}
		| call BRACKET_L expr BRACKET_R {}
		;

call:
		call PAREN_L elist PAREN_R {printf("funccall(<elist>)\n");}
		| lvalue callsuffix {}
		| PAREN_L funcdef PAREN_R PAREN_L elist PAREN_R {}
		;

callsuffix:
		normcall {printf("funcall %s()\n",$$);}
		| methodcall {}
		;

normcall:
		PAREN_L elist PAREN_R {}
		;

methodcall:
		DDOT IDENTIFIER PAREN_L elist PAREN_R {}
		;

objectdef:
		BRACKET_L elist BRACKET_R {}
		| BRACKET_L indexed BRACKET_R {}
		;

indexed:
		indexedelem {}
		| indexed COMMA indexedelem {}
		;

indexedelem:
		BRACE_L expr COLON expr BRACE_R {}
		;

elist:
		expr {}
		| elist COMMA expr {}
		| {}
		;
 
func_temp:
		IDENTIFIER{push(&func_names,$1);} PAREN_L{
			scope_main++;
			in_func=1;
			func_started=1;
			func_scope++;
			 

		} 
		idlist PAREN_R  block { func_scope--;in_func=0;pop(&func_names);printf("Func <id> (<parameters>) \n");}
		| PAREN_L{
		 
 
		 	// We increase the scope of the function
			func_scope++;

			// We set that we are in a function
			in_func=1;

			// ...and the function has just started.
			func_started=1;

			// We generate a function name for this function
			// and insert the symbol to the symbol table
		 	st_entry * se = NULL;
		 	temp_str = generate_func_name(func_signed);
			se = create_symbol(temp_str,1,scope_main,yylineno,USERFUNC);
			st_insert((symbol_table **)st,&se);

			func_signed++;
			scope_main++;

		}idlist PAREN_R{ } block { func_var=0;func_scope--;in_func=0;printf("Func (<parameters>) \n");}
		; 

funcdef:
		FUNCTION{func_var=1;printf("FUNCTION\n");} func_temp
		;

idlist:
		IDENTIFIER {printf("fun par:%s\n",$1);}
		| idlist COMMA IDENTIFIER {printf("fun par:%s \n",$3);}
		| 
		;

block:
		BRACE_L {
			if(!func_started)scope_main++;
			else func_started = 0;
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
		IF PAREN_L expr PAREN_R stmt %prec IF_TERM { printf("if (<expr>) <stmt>\n");}
		| IF PAREN_L expr PAREN_R stmt ELSE stmt { printf("if (<expr>) <stmt> else <stmt>\n");}
		;

whilestmt:
		WHILE {scope_loop++;} PAREN_L expr PAREN_R stmt{scope_loop--;} {
			printf("while (<expr>) <stmt>\n");
	 
		}
		;

forstmt:
		FOR {scope_loop++;} PAREN_L elist SEMICOLON expr SEMICOLON elist PAREN_R stmt {
			printf("for (<elist>;<expr>;<elist>) <stmt>\n ");
			scope_loop--;
		}
		;

returnstmt:
		RETURN SEMICOLON {
			if(func_scope==0)yyerror("Cannot use return; when not in a function.");
			else printf("return ;\n");
		}
		| RETURN expr SEMICOLON {
			if(func_scope==0)yyerror("Cannot use return; when not in a function.");
			else printf("return <expr>;\n");
		}
		;

%%

int yyerror (const char * yaccProvideMessage){
	fprintf(stderr,"Error at line %d: %s\n",yylineno,yaccProvideMessage);
}

int main(int argc,char ** argv)
{
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

	yyparse(&st);
	printf("\n <--[Parsing Completed]-->\n");
	printf("Press [Enter] to continue with the symbol table.\n");
	getchar();
	print_st(st);
	return 0;	
}
