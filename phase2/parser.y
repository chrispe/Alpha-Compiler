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
		| {printf("\n <--[Parsing Started]-->\n\n");}
		;

stmt:
		expr SEMICOLON {}
		| BREAK SEMICOLON {printf("break;\n");}
		| CONTINUE SEMICOLON {printf("continue;\n");}
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
		|   expr EQ_LESS expr {printf("<expr> <= <expr>d\n",$1,$3);$$ = ($1 <= $3)?1:0;}
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
		IDENTIFIER {printf("ID\n");}
		| LOCAL IDENTIFIER {printf("LOCAL ID\n");}
		| DCOLON IDENTIFIER {printf("::ID\n");}
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
		FUNCTION IDENTIFIER PAREN_L idlist PAREN_R block {printf("Func <id> (<parameters>) \n");}
		| FUNCTION PAREN_L idlist PAREN_R block {printf("Func (<parameters>) \n");}
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
		IF PAREN_L expr PAREN_R stmt %prec IF_TERM {printf("if (<expr>) <stmt>\n");}
		| IF PAREN_L expr PAREN_R stmt ELSE stmt {printf("if (<expr>) <stmt> else <stmt>\n");}
		;

whilestmt:
		WHILE PAREN_L expr PAREN_R stmt {printf("while (<expr>) <stmt>\n");}
		;

forstmt:
		FOR PAREN_L elist SEMICOLON expr SEMICOLON elist PAREN_R stmt {printf("for (<elist>;<expr>;<elist>) <stmt>\n ");}
		;

returnstmt:
		RETURN SEMICOLON {printf("return ;\n");}
		| RETURN expr SEMICOLON {printf("return <expr>;\n");}
		;

%%

int yyerror (const char * yaccProvideMessage){
	fprintf(stderr,"Error at line %d: %s\n",yylineno,yaccProvideMessage);
}

int main(int argc,char ** argv)
{
	/* Testing */
 	symbol_table * st;
 	st_entry * s1;
 	st_entry * s2;
 	st_entry * s3;
 	st_entry * s4;

 	s1 = create_symbol("a",0, 0, 0,LIBFUNC);
 	s2 = create_symbol("b",0, 0, 0,GLOBAL);
 	s3 = create_symbol("c",0, 0, 0,GLOBAL);
 	s4 = create_symbol("d",0, 0, 0,GLOBAL);
	s1 = set_var_func(s1,"dds");

	args_insert(&(s1->value_type.funVal->arguments),"const");
	args_insert(&(s1->value_type.funVal->arguments),"dsst");
	args_insert(&(s1->value_type.funVal->arguments),"cdsst");

 	st = create_symbol_table();

	st_insert(&st,&s1);
	st_insert(&st,&s2);
	st_insert(&st,&s3);
	st_insert(&st,&s4);

	print_st(st);
	/* Test end */

    if (argc > 1) {
        if ((yyin = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "Cannot read file %s\n", argv[1]);
            return 1;
        }
    }
    else
        yyin = stdin;

	yyparse();
	printf("\n <--[Parsing Completed]-->\n");
	return 0;	
}