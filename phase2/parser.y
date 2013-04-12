%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
 	#include "parser_lib.h"
	#define YYPARSE_PARAM st
	
	int yyerror (const char * yaccProvideMessage);
	
	// Importing variables from yylex.
	extern int yylex(void);
	extern int yylineno;
	extern char * yytext;
	extern FILE * yyin;
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
		expr SEMICOLON { fun_rec=0; }
		| BREAK SEMICOLON {
			if(scope_loop>0)
				printf("break;\n");
			else
				yyerror("Cannot use break; outside of a loop.");
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
		lvalue EQUAL{expr_started=1;printf("Expr started\n");} 
		expr { 
			expr_started=0; 
			if(fun_rec)
				printf("Error at line %d: '%s' is a declared function, cannot assign to a function.\n",yylineno,$$);
			fun_rec=0;
		}
		;

lvalue:
		IDENTIFIER {

			// Adding the id to the symbol table.
			// Every required checking is included in the following function.
			add_variable((symbol_table **)st, $$,yylineno);
 		 			
		}
		| LOCAL IDENTIFIER {

			// Adding the local id to the symbol table.
			// Every required checking is included in the following function.
			add_local_variable((symbol_table **)st, $2,yylineno);

		}
		| DCOLON IDENTIFIER {

			// We check that the global variable exists
			// The whole proccess is handled by the following funciton.
			check_global_variable((symbol_table **)st, $2,yylineno);

		}
		| member {}
		;

member:
		lvalue DOT IDENTIFIER {printf("lvalue.id\n");}
		| lvalue BRACKET_L expr BRACKET_R {printf("lvalue[expr]\n");}
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
		DDOT IDENTIFIER PAREN_L elist PAREN_R {printf("..id(elist)");}
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
		IDENTIFIER{

			// Adding the function(with name) to the symbol table.
			// Every required checking is included in the following method.
			add_function((symbol_table **)st,$1,yylineno,1);

		} 
		PAREN_L {
			scope_main++;
			in_func=1;
			func_started=1;
			func_scope++;
		} 
		idlist PAREN_R  block { func_scope--;in_func=0;pop(&func_names);printf("Func <id> (<parameters>) \n");}
		| PAREN_L{
		 
 			// Adding the function(without name) to the symbol table.
			// Every required checking is included in the following method.
			add_function((symbol_table **)st,NULL,yylineno,0);
 

		} idlist PAREN_R{ } block { func_var=0;func_scope--;in_func=0;pop(&func_names);printf("Func (<parameters>) \n");}
		; 

funcdef:
		FUNCTION{func_var=1;printf("FUNCTION\n");} func_temp
		;

idlist:
		IDENTIFIER {
 
 			// Adding the argument of a function to the symbol table.
			// Every required checking is included in the following method.
 			add_function_argument((symbol_table **)st,$1,yylineno,0);

		}
		| idlist COMMA IDENTIFIER {

			// Adding the argument of a function to the symbol table.
			// Every required checking is included in the following method.
			add_function_argument((symbol_table **)st,$3,yylineno,1);

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
		IF PAREN_L expr PAREN_R stmt %prec IF_TERM { printf("if (<expr>) <stmt>\n"); }
		| IF PAREN_L expr PAREN_R stmt ELSE stmt { printf("if (<expr>) <stmt> else <stmt>\n"); }
		;

whilestmt:
		WHILE {scope_loop++;} PAREN_L expr PAREN_R stmt {scope_loop--;} {
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