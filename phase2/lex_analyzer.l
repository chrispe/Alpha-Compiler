%{
    #define KEYWORDS    15
    #define OPERATORS   14
    #define PUNCTUATION 12
    #define YACC_DEFINED_START 262
    #define TOTAL_TYPES (KEYWORDS + OPERATORS + PUNCTUATION)
    #include "lex_lib.h"
    #include "parser.h"
%}

IDENTIFIER     [a-zA-Z][a-zA-Z0-9_]*

REAL_E         [0-9]+((e|E)([/+|/-][0-9]+)+)

REAL           [0-9]+\.[0-9]+((e[/+|/-]|E[/+|/-])[0-9]+)?

INTEGER_HEX    (0[xX][0-9A-Fa-f]+)

INTEGER_DEC    [0-9]*

STRING         \"(\\\"|.)*\"

WRONG_STR      \"

OPERATOR       "="|"+"|"-"|"*"|"/"|"%"|"=="|"!="|"++"|"--"|">"|"<"|">="|"<="

PUNCT          "{"|"}"|"["|"]"|"("|")"|";"|","|":"|"::"|"."|".."

SPACE          [\r\n \t\v]

MULTI_COMMENT  "/*"

KEYWORD        if|else|while|for|function|return|break|continue|and|not|or|local|true|false|nil

SINGLE_COMMENT "//".*

INVALID_ID     [0-9][a-zA-Z0-9_]*

UNKNOWN        .

%option yylineno
%option lex-compat
%option noyywrap
%%

{STRING} {

    char *tmp = malloc(sizeof(yytext));
    char *p   = tmp; 
    strcpy(tmp, yytext);
    int no_error = 1;

    while (*p++) {
        if (*p == '\\') {
            if (p[1] == 't')
                replace(p, "\\t", '\t');
            else if (p[1] == 'n')
                replace(p, "\\n", '\n');
            else if (p[1] == '"')
                replace(p, "\"", '"');    
            else if (p[1] == '\\')
                replace(p, "\\\\", '\\');
            else {
                fprintf(stderr, "Error on line %d: Unrecognized escape character.\n", yylineno);
                no_error = 0;
                break;
            } 
        }
    }
    if(no_error)
    {
        yylval.strval = strdup(tmp);
        return STRING;
    }
}

{WRONG_STR} {
    int c;
    while(c!=EOF && c!='\n')c=input();
    fprintf(stderr, "Error on line %d: Unclosed string.\n", yylineno);
    return EOF;
}

{KEYWORD} {
    enum yytokentype type;
    char *key = yytext;
    int i;

    for (i = 0; i < KEYWORDS; i++) {
        if (!strcmp(key_str[i], key)) {
            type = i;
            break;
        }
    }
    return type + YACC_DEFINED_START;
}

{OPERATOR} {
    enum yytokentype type;
    char *op = yytext;
    int i;

    for (i = 0; i < OPERATORS; i++) {
        if (!strcmp(op_str[i], op)) {
            type = i + KEYWORDS;
            break;
        }
    }
    return type + YACC_DEFINED_START;
}

{INTEGER_HEX} {
    int val = htoi(yytext);
    yylval.intval = val; 
    return INTEGER;
}

{INTEGER_DEC} {
    int val = atoi(yytext);
    yylval.intval = val; 
    return INTEGER;
}

{REAL} {
    double val = atof(yytext);
    yylval.fltval = val; 
    return REAL;
}

{REAL_E} {
    double val = atof(yytext);
    yylval.fltval = val;
    return REAL;
}

{PUNCT} {
    enum yytokentype type;
    char *pun = yytext;
    int i;

    for (i = 0; i < PUNCTUATION; i++) {
        if (!strcmp(pun_str[i], pun)) {
            type = i + KEYWORDS + OPERATORS +  3;
            break;
        }
    }
    return type + YACC_DEFINED_START;
}

{IDENTIFIER} {
    yylval.strval = strdup(yytext);
    return IDENTIFIER;
}

{SINGLE_COMMENT} {}

{MULTI_COMMENT} {
    int c;
    int b = 1;
    int current_line = yylineno;

    while ((c = input()) != EOF) {
        if (c == '*') {
            if ((c = input()) == '/') {
                b--;
                if (b <= 0)
                    break;
            }
            else unput(c);
        }
        else if (c == '/') {
            if ((c = input()) == '*')
                b++;
        }
    }

    if (b != 0) {
        fprintf(stderr,"Error: Unclosed comment starting on line %d.\n", current_line);
        return EOF;
    }
}

{SPACE} {}

{INVALID_ID} { fprintf(stderr,"Error on line %d: Identifiers cannot start with a number.\n", yylineno);}

{UNKNOWN} { fprintf(stderr,"Error on line %d: Unrecognized character '%s'.\n", yylineno,yytext);}

<<EOF>> { return EOF; }
    
%%