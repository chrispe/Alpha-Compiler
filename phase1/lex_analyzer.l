%{
    #define YY_DECL int alpha_yylex(void ** yyal)
    #define KEYWORDS    15
    #define OPERATORS   14
    #define PUNCTUATION 12
    #define TOTAL_TYPES (KEYWORDS + OPERATORS + PUNCTUATION)
    #include "token.h"
 
    alpha_token_t *tail = NULL;
    int errors = 0;

%}

IDENTIFIER     [a-zA-Z][a-zA-Z0-9_]*

REAL_E         [0-9]+((e|E)([/+|/-][0-9]+)?)

REAL           [0-9]*\.[0-9]*((e[/+|/-]|E[/+|/-])[0-9]+)?

INTEGER_HEX    (0[xX][0-9A-Fa-f]+)

INTEGER_DEC    [0-9]*

STRING         \"(\\\"|.)*\"

WRONG_STR      \"

OPERATOR       "="|"+"|"-"|"*"|"/"|"%"|"=="|"!="|"++"|"--"|">"|"<"|">="|"<="

PUNCT          "{"|"}"|"["|"]"|"("|")"|";"|","|":"|"::"

SPACE          [\r\n \t\v]

MULTI_COMMENT  "/*"

KEYWORD        if|else|while|for|function|return|break|continue|and|not|or|local|true|false|nil

SINGLE_COMMENT "//".*

INVALID_ID     [0-9][a-zA-Z0-9_]*

UNKNOWN        .

%option yylineno
%option lex-compat
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
            else if (p[1] == '\\')
                replace(p, "\\\\", '\\');
            else {
                fprintf(stderr, "Error on line %d: Unrecognized escape character.\n", yylineno);
                no_error = 0;
                errors++;
                break;
            } 
        }
    }
    if(no_error)addToken((alpha_token_t **)yyal, &tail, yylineno, STRING, tmp);
}

{WRONG_STR} {
    errors++;
    int c;
    while(c!=EOF && c!='\n')c=input();
    fprintf(stderr, "Error on line %d: Unclosed string.\n", yylineno);
}

{KEYWORD} {
    token_type_t type;
    char *key = yytext;
    int i;

    for (i = 0; i < KEYWORDS; i++) {
        if (!strcmp(key_str[i], key)) {
            type = i;
            break;
        }
    }
    addToken((alpha_token_t **)yyal, &tail, yylineno, type, yytext);
}

{OPERATOR} {
    token_type_t type;
    char *op = yytext;
    int i;

    for (i = 0; i < OPERATORS; i++) {
        if (!strcmp(op_str[i], op)) {
            type = i + KEYWORDS;
            break;
        }
    }
    addToken((alpha_token_t **)yyal, &tail, yylineno, type, yytext);
}

{INTEGER_HEX} {
    int val = htoi(yytext);
    addToken((alpha_token_t **)yyal, &tail, yylineno, INTEGER, &val);
}

{INTEGER_DEC} {
    int val = atoi(yytext);
    addToken((alpha_token_t **)yyal, &tail, yylineno, INTEGER, &val);
}

{REAL} {
    double val = atof(yytext);
    addToken((alpha_token_t **)yyal, &tail, yylineno, REAL, &val);
}

{REAL_E} {
    double val = atof(yytext);
    addToken((alpha_token_t **)yyal, &tail, yylineno, REAL, &val);
}

{PUNCT} {
    token_type_t type;
    char *pun = yytext;
    int i;

    for (i = 0; i < PUNCTUATION; i++) {
        if (!strcmp(pun_str[i], pun)) {
            type = i + KEYWORDS + OPERATORS +  3;
            break;
        }
    }
    addToken((alpha_token_t **)yyal, &tail, yylineno, type, yytext);
}

{IDENTIFIER} {
    addToken((alpha_token_t **)yyal, &tail, yylineno, IDENTIFIER, yytext);
}

{SINGLE_COMMENT} {
    addToken((alpha_token_t **)yyal, &tail, yylineno, SINGLE_COMMENT, yytext);
}

{MULTI_COMMENT} {

        int c;
        int cline = yylineno;
        int ends = 0;
        char * comment = NULL;

        while((c=input())!=EOF){
            if(c=='*'){
                if((c=input())=='/'){
                    ends = 1;
                    break;
                }
                comment = extendString(comment,'*');
            }
            comment = extendString(comment,c);
        }   
        if(!ends){
            fprintf(stderr,"\nError on line %d: Unclosed comment\n",cline);
            errors++;
        }
        else addToken((alpha_token_t **)yyal, &tail, yylineno, MCOMMENT, comment);
}

{SPACE} {}

{INVALID_ID} { fprintf(stderr,"Error on line %d: Identifiers cannot start with a number.\n", yylineno);errors++;}

{UNKNOWN} { fprintf(stderr,"Error on line %d: Unrecognized character\n", yylineno);errors++;}

<<EOF>> { return 1; }
    
%%
int main(int argc, char *argv[])
{
    alpha_token_t *head = NULL;
    if (argc > 1) {
        if ((yyin = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "Cannot read file %s\n", argv[1]);
            return 1;
        }
    }
    else
        yyin = stdin;
    
    alpha_yylex((void **)&head);
    if(errors){printf("Some errors (%d) occured, press ENTER to continue with the analysis results.\n",errors);getchar();}
    printTokens(head);
    return 0;
}