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

 	// A number which indicates how many function we have set by the prefix '$_(id)'
 	unsigned int func_signed = 0;

 	// A boolean which indicates if a function name has been called.
 	char is_func_id = 0;
 	char func_started = 0;

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
%type <strval> elist objectdef indexedelem indexed funcdef idlist block ifstmt block_in whilestmt forstmt
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
		expr SEMICOLON {}
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
					if($3!=0)$$ = (int)$1 / (int)$3;
					else yyerror("Cannot divide by zero.");
			}
		|	expr PERCENT expr {
					printf("<expr> %% <expr>\n",$1,$3);
					if($3!=0)$$ = (int)$1 % (int)$3;
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
		lvalue EQUAL expr {}
		;

lvalue:
		IDENTIFIER {

 			int i;
 			st_entry * se = NULL;

 			// Lookup symbol table starting at the current 
 			// scope and going one level down in each loop
 			// until we get to know if the symbol exists.

			for(i=scope_main;i>=0;i--){
				se = st_lookup_scope(*((symbol_table **)st),$$,i);
				if(se!=NULL)break;
			}

			// If the symbol was found then we need to detect if we have access to it.
			if(se!=NULL){

				//In case we are in a function and we try to access a non-global variable
				if(in_func & se->scope!=0 && se->type!=USERFUNC &&  se->type!=LIBFUNC)
					printf("Error at line %d: Variable '%s' not accessible.\n",yylineno,$$);
				else if(se->active && se->type==USERFUNC)
					printf("Error at line %d: '%s' is an activated user function, cannot be shadowed.\n",yylineno,$$);
				else if(se->type==LIBFUNC)
					printf("Error at line %d: '%s' is a library function, must not be shadowed.\n",yylineno,$$);
				else printf("Variable '%s' was detected and used.\n", $$);
			}
			else {
				// Else if the symbol could not be found we insert it in the symbol table.
				se = create_symbol($$,1,scope_main,yylineno,VAR);
				st_insert((symbol_table **)st,&se);
				printf("Added variable '%s' in the symbol table.\n",$$);
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
					se = create_symbol($2,1,scope_main,yylineno,LCAL);
					printf("Added local variable '%s' in the symbol table.\n",$2);
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
		;

callsuffix:
		normcall {}
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
 
funcdef:
		FUNCTION IDENTIFIER PAREN_L{scope_main++;in_func=1;func_started=1;} idlist PAREN_R block { in_func=0;printf("Func <id> (<parameters>) \n");}
		| FUNCTION PAREN_L{scope_main++;in_func=1;func_started=1;}  idlist PAREN_R block { in_func=0;printf("Func (<parameters>) \n");}
		;

idlist:
		IDENTIFIER {}
		| idlist COMMA IDENTIFIER {}
		| {}
		;

block:
		BRACE_L {
			if(!func_started)scope_main++;
			else func_started = 0;
		} block_in BRACE_R {
			// We de-active the local variables of the current scope
			scope_set_active((symbol_table **)st,scope_main,0);
			scope_main--;
		}
		;
 
block_in:	
		block_in stmt {}
		| {}
		;

ifstmt:
		IF PAREN_L expr PAREN_R stmt %prec IF_TERM {scope_main++;printf("if (<expr>) <stmt>\n");}
		| IF PAREN_L expr PAREN_R stmt ELSE stmt {scope_main++;printf("if (<expr>) <stmt> else <stmt>\n");}
		;

whilestmt:
		WHILE PAREN_L  expr PAREN_R stmt {
			printf("while (<expr>) <stmt>\n");
	 
		}
		;

forstmt:
		FOR PAREN_L elist SEMICOLON expr SEMICOLON elist PAREN_R stmt {
			printf("for (<elist>;<expr>;<elist>) <stmt>\n ");
			 
		}
		;

returnstmt:
		RETURN SEMICOLON {
			if(!in_func)yyerror("Cannot use return; when not in a function.");
			else printf("return ;\n");
		}
		| RETURN expr SEMICOLON {
			if(!in_func)yyerror("Cannot use return; when not in a function.");
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
