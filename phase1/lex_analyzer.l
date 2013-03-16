%{
    #define YY_DECL int alpha_yylex(void *yyal)
    #define KEYWORDS    15
    #define OPERATORS   14
    #define PUNCTUATION 12
    #define TOTAL_TYPES (KEYWORDS + OPERATORS + PUNCTUATION)
    #include "token.h"
 
    alpha_token_t *head = NULL;
    alpha_token_t *tail = NULL;
%}

IDENTIFIER     [a-zA-Z][a-zA-Z0-9_]*

INTEGER_HEX    (0[xX][0-9A-Fa-f]+)

INTEGER_DEC    [0-9]*

REAL           [0-9]*\.[0-9](e|E)?[0-9]*

STRING         \"(\\\"|.)*\"

OPERATOR       "="|"+"|"-"|"*"|"/"|"%"|"=="|"!="|"++"|"--"|">"|"<"|">="|"<="

PUNCT          "{"|"}"|"["|"]"|"("|")"|";"|","|":"|"::"

SPACE          [\r\n \t\v]

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
                break;
            } 
        }
    }

    if(no_error)addToken(&head, &tail, yylineno, STRING, tmp);
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
    addToken(&head, &tail, yylineno, type, yytext);
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
    addToken(&head, &tail, yylineno, type, yytext);
}

{INTEGER_HEX} {
    int val = htoi(yytext);
    addToken(&head, &tail, yylineno, INTEGER, &val);
}

{INTEGER_DEC} {
    int val = atoi(yytext);
    addToken(&head, &tail, yylineno, INTEGER, &val);
}

{REAL} {
    double val = atof(yytext);
    addToken(&head, &tail, yylineno, REAL, &val);
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

    addToken(&head, &tail, yylineno, type, yytext);
}

{IDENTIFIER} {
    addToken(&head, &tail, yylineno, IDENTIFIER, yytext);
}

{SINGLE_COMMENT} {
    addToken(&head, &tail, yylineno, SINGLE_COMMENT, yytext);
}

{SPACE} {}

"/*".* {
        int c;
        int dev = 1;

        while((c=input())!=EOF)
        {
            if(c=='*')
            {
                if((c=input())=='/')
                {
                    dev--;
                    if(dev<0)break;
                }
                unput(c);
            }
            else if(c=='/')
            {
                if((c=input())=='*')dev++;
                else unput(c);
            }
        }   
        printf("Added comment : %s",yytext);
        if(dev!=0)
        {
                printf("\nError on line %d: Unclosed comment\n",yylineno);
                return(-1);
        }
}

{INVALID_ID} { printf("Error on line %d: Identifiers cannot start with a number.\n", yylineno);}

{UNKNOWN} { printf("Error on line %d: Unrecognized character\n", yylineno);}

<<EOF>> { printf("\nTotal lines: %d\n", yylineno); return 1; }
    
%%
int main(int argc, char *argv[])
{
    if (argc > 1) {
        if ((yyin = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "Cannot read file %s\n", argv[1]);
            return 1;
        }
    }
    else
        yyin = stdin;
    
    alpha_yylex(head);
    printTokens(head);
    return 0;
}