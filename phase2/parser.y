%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "symbol_table.h"
	int yyerror (char * yaccProvideMessage);
	extern int yylex(void);
	extern int yylineno;
	extern char * yytext;
	extern FILE * yyin;
%}

%start program
%defines 

%union{
	int intval;
	double fltval;
	char * strval;
}

%token <intval> INTEGER;
%token <fltval> REAL;
%token <strval> STRING;
%token <strval> IDENTIFIER;


%% 

program:

%%

int yyerror (char * yaccProvideMessage){
	fprintf(stderr,"Error at line %d: %s\n",yylineno,yaccProvideMessage);
}

int main(int argc,char ** argv)
{
	symbol_table * sb;
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