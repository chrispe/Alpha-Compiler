%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
 	#include "symbol_table.h"
	int yyerror (const char * yaccProvideMessage);
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

%type <strval> stmt;
%type <fltval> expr term;
 

%left OR
%left AND
%left DEQUAL NEQUAL
%left GREATER EQ_GREATER LESS EQ_LESS
%left PLUS MINUS
%left MULTI SLASH PERCENT
%left NOT UMINUS


 
%% 

program:
		program stmt
		|
		;

stmt:
		expr SEMICOLON {printf("Expression result : %f\n",$1);}
		;

expr:
		REAL				{$$ = $1;}
		|   INTEGER			{$$ = $1;}
		|	expr PLUS expr 	{$$ = $1 + $3;}
		|	expr MINUS expr	{$$ = $1 - $3;}
		|	expr MULTI expr {$$ = $1 * $3;}
		|	expr SLASH expr {
					if($3!=0)$$ = (int)$1 / (int)$3;
					else yyerror("Cannot divide by zero.");
			}
		|	expr PERCENT expr {
					if($3!=0)$$ = (int)$1 % (int)$3;
					else yyerror("Cannot divide by zero.");
			}
		|	expr GREATER expr {$$ = ($1 > $3)?1:0;}
		|	expr EQ_GREATER expr {$$ = ($1 >= $3)?1:0;}
		|	expr LESS expr {$$ = ($1 < $3)?1:0;}
		|   expr EQ_LESS expr {$$ = ($1 <= $3)?1:0;}
		|	expr DEQUAL expr {$$ = ($1 == $3)?1:0;}
		|	expr NEQUAL expr {$$ = ($1 != $3)?1:0;}
		|	expr AND expr {$$ = ($1 && $3)?1:0;}
		|	expr OR expr {$$ = ($1 || $3)?1:0;}
		| 	term {}
		;

term:
		PAREN_L expr PAREN_R	{ $$ = $2; }
		| MINUS expr %prec UMINUS { $$ = $2*(-1);}
		;
%%

int yyerror (const char * yaccProvideMessage){
	fprintf(stderr,"Error at line %d: %s\n",yylineno,yaccProvideMessage);
}

int main(int argc,char ** argv)
{
 
    if (argc > 1) {
        if ((yyin = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "Cannot read file %s\n", argv[1]);
            return 1;
        }
    }
    else
        yyin = stdin;

	yyparse();
	return 0;	
}