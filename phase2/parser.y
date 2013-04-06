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

%type <strval> stmt assignexpr lvalue const primary member call callsuffix normcall methodcall   term
%type <strval> elist objectdef indexedelem indexed funcdef idlist block ifstmt block_in whilestmt forstmt
%type <intval> expr  
 
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
		| {printf("Parsing file...\n");}
		;

stmt:
		expr SEMICOLON {}
		| BREAK SEMICOLON {}
		| CONTINUE SEMICOLON {}
		| forstmt {}
		| whilestmt {}
		| block {}
		| ifstmt {}
		| funcdef {}
		| returnstmt {}
		| SEMICOLON {}
		;

expr:
		assignexpr {/* empty action */}
		|	expr PLUS expr 	{printf("%d+%d\n",$1,$3); $$ = $1 + $3;}
		|	expr MINUS expr	{printf("%d-%d\n",$1,$3); $$ = $1 - $3;}
		|	expr MULTI expr {printf("%d*%d\n",$1,$3); $$ = $1 * $3;}
		|	expr SLASH expr {
					printf("%d/%d\n",$1,$3);
					if($3!=0)$$ = (int)$1 / (int)$3;
					else yyerror("Cannot divide by zero.");
			}
		|	expr PERCENT expr {
					printf("%d%%%d\n",$1,$3);
					if($3!=0)$$ = (int)$1 % (int)$3;
					else yyerror("Cannot divide by zero.");
			}
		|	expr GREATER expr {printf("%d>%d\n",$1,$3); $$ = ($1 > $3)?1:0;}
		|	expr EQ_GREATER expr {printf("%d>=%d\n",$1,$3); $$ = ($1 >= $3)?1:0;}
		|	expr LESS expr {printf("%d%d\n",$1,$3); $$ = ($1 < $3)?1:0;}
		|   expr EQ_LESS expr {printf("%d<=%dd\n",$1,$3); $$ = ($1 <= $3)?1:0;}
		|	expr DEQUAL expr {printf("%d==%d\n",$1,$3); $$ = ($1 == $3)?1:0;}
		|	expr NEQUAL expr {printf("%d!=%d\n",$1,$3); $$ = ($1 != $3)?1:0;}
		|	expr AND expr {printf("%d and %d\n",$1,$3); $$ = ($1 && $3)?1:0;}
		|	expr OR expr {printf("%d or %d\n",$1,$3); $$ = ($1 || $3)?1:0;}
		| 	term {/* empty action */}
		;

term:
		PAREN_L expr PAREN_R	{}
		| MINUS expr %prec UMINUS {}
		| NOT expr {}
		| lvalue DPLUS {}
		| DPLUS lvalue {}
		| lvalue DMINUS {}
		| DMINUS lvalue {}
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
		REAL	{printf("float : %f",$1);}
		| INTEGER {}
		| STRING {}
		| NIL {}
		| TRUE {}
		| FALSE {}
		;

assignexpr:
		lvalue EQUAL expr {printf("Expression\n");}
		;

lvalue:
		IDENTIFIER {}
		| LOCAL IDENTIFIER {}
		| DCOLON IDENTIFIER {}
		| member {}
		;

member:
		lvalue DOT IDENTIFIER {}
		| lvalue BRACKET_L expr BRACKET_R {}
		| call DOT IDENTIFIER {}
		| call BRACKET_L expr BRACKET_R {}
		;

call:
		call PAREN_L elist PAREN_R {}
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
		FUNCTION IDENTIFIER PAREN_L idlist PAREN_R block {}
		| FUNCTION PAREN_L idlist PAREN_R block {}
		;

idlist:
		IDENTIFIER {}
		| idlist COMMA IDENTIFIER {}
		| {}
		;

block:
		BRACE_L block_in BRACE_R {}
		;
 
block_in:	
		block_in stmt {}
		| {}
		;

ifstmt:
		IF PAREN_L expr PAREN_R stmt %prec IF_TERM {}
		| IF PAREN_L expr PAREN_R stmt ELSE stmt {}
		;

whilestmt:
		WHILE PAREN_L expr PAREN_R stmt {}
		;

forstmt:
		FOR PAREN_L elist SEMICOLON expr SEMICOLON elist PAREN_R stmt {}
		;

returnstmt:
		RETURN SEMICOLON {}
		| RETURN expr SEMICOLON {}
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