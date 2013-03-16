%{
    #define YY_DECL int alpha_yylex(void *yyal)
    #define KEYWORDS    15
    #define OPERATORS   14
    #define PUNCTUATION 12
    #define TOTAL_TYPES (KEYWORDS + OPERATORS + PUNCTUATION)
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    /* auta 8a ta xwsoume s'enan .h gia na exoume omorfo kwdika */
    char *token_type_str[] = {
        "KEYWORD_IF", "KEYWORD_ELSE", "KEYWORD_WHILE", "KEYWORD_FOR", "KEYWORD_FUNCTION",
        "KEYWORD_RETURN", "KEYWORD_BREAK", "KEYWORD_CONTINUE", "KEYWORD_AND", "KEYWORD_NOT",
        "KEYWORD_OR", "KEYWORD_LOCAL", "KEYWORD_TRUE", "KEYWORD_FALSE", "KEYWORD_NIL", "OP_EQUAL",
        "OP_PLUS", "OP_MINUS", "OP_ASTERISK", "OP_SLASH", "OP_PERCENT", 
        "OP_EQ", "OP_NEQ", "OP_INC", "OP_DEC", "OP_GT", "OP_LT", "OP_EGT",
        "OP_ELT", "INTEGER", "REAL", "STRING", "PUN_BRACE_L", "PUN_BRACE_R",
        "PUN_BRACKET_L", "PUN_BRACKET_R", "PUN_PAREN_L", "PUN_PAREN_R",
        "PUN_SCOLON", "PUN_COMMA", "PUN_COLON", "PUN_DCOLON", "PUN_DOT",
        "PUN_DDOT", "IDENTIFIER", "SINGLE_COMMENT", "MCOMMENT"
    };
    
    char *key_str[] = {
        "if", "else", "while", "for", "function",
        "return", "break", "continue", "and", "not",
        "or", "local", "true", "false", "nil"
    };

    char *op_str[] = {
        "=", "+", "-", "*", "/", "%", "==", "!=",
        "++", "--", ">", "<", "=>", "<="
    };

    char *pun_str[] = {
        "{", "}", "[", "]", "(", ")", ";", ",", ":", "::", ".", ".."
    };

    typedef enum {
        KEYWORD_IF,
        KEYWORD_ELSE,
        KEYWORD_WHILE,
        KEYWORD_FOR,
        KEYWORD_FUNCTION,
        KEYWORD_RETURN,
        KEYWORD_BREAK,
        KEYWORD_CONTINUE,
        KEYWORD_AND,
        KEYWORD_NOT,
        KEYWORD_OR,
        KEYWORD_LOCAL,
        KEYWORD_TRUE,
        KEYWORD_FALSE,
        KEYWORD_NIL,
        OP_EQUAL,
        OP_PLUS,
        OP_MINUS,
        OP_ASTERISK,
        OP_SLASH,
        OP_PERCENT,
        OP_EQ,
        OP_NEQ,
        OP_INC,
        OP_DEC,
        OP_GT,
        OP_LT,
        OP_EGT,
        OP_ELT,
        INTEGER,
        REAL,
        STRING,
        PUN_BRACE_L,
        PUN_BRACE_R,
        PUN_BRACKET_L,
        PUN_BRACKET_R,
        PUN_PAREN_L,
        PUN_PAREN_R,
        PUN_SCOLON,
        PUN_COMMA,
        PUN_COLON,
        PUN_DCOLON,
        PUN_DOT,
        PUN_DDOT,
        IDENTIFIER,
        SINGLE_COMMENT,
        MCOMMENT
    } token_type_t;
        
    typedef struct alpha_token {
        unsigned int line;
        token_type_t type;
        void        *content;
        struct alpha_token *next;
    } alpha_token_t;
        
    /* proswrina */
    alpha_token_t *head = NULL;
    alpha_token_t *tail = NULL;

    void replace(char *, char *, char);
    void addToken(alpha_token_t **, alpha_token_t **, unsigned int, token_type_t, void *);
%}

IDENTIFIER     [a-zA-Z][a-zA-Z0-9_]*

INTEGER        (0[xX][0-9A-Fa-f]+)|[0-9]*

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
    
    while (*p++) {
        if (*p == '\\') {
            if (p[1] == 't')
                replace(p, "\\t", '\t');
            else if (p[1] == 'n')
                replace(p, "\\n", '\n');
            else if (p[1] == '\\')
                replace(p, "\\\\", '\\');
            else {
                fprintf(stderr, "Error on line %d: Stop being an idiot.\n", yylineno);
                return -1;
            } 
        }
    }
    printf("string: %s\n", tmp);
    addToken(&head, &tail, yylineno, STRING, tmp);
}

{KEYWORD} {
    printf("keyword: %s\n", yytext);
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
    printf("operator: %s\n", yytext);
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

{INTEGER} {
    printf("integer: %s\n", yytext);
    /* todo: metatropes vasewn */
    int val = atoi(yytext);
    addToken(&head, &tail, yylineno, INTEGER, &val);
}

{REAL} {
    printf("real: %s\n", yytext);
    double val = atof(yytext);
    addToken(&head, &tail, yylineno, REAL, &val);
}

{PUNCT} {
    printf("punct: %s\n", yytext);
    token_type_t type;
    char *pun = yytext;
    int i;

    for (i = 0; i < PUNCTUATION; i++) {
        if (!strcmp(pun_str[i], pun)) {
            /* O_O */
            type = i + KEYWORDS + OPERATORS +  3;
            break;
        }
    }

    addToken(&head, &tail, yylineno, type, yytext);
}

{IDENTIFIER} {
    printf("id: %s\n", yytext);
    addToken(&head, &tail, yylineno, IDENTIFIER, yytext);
}

{SINGLE_COMMENT} {
    printf("comment: %s\n", yytext);
    addToken(&head, &tail, yylineno, SINGLE_COMMENT, yytext);
}

{SPACE} {}

"/*" {
    int c;
    int b = 1;
    
    while ((c = input()) != EOF) {
        if (c == '*') {
            if ((c = input()) == '/') {
                b--;
                if (b < 0)
                    break;
            }
            unput(c);
        }
        else if (c == '/') {
            if ((c = input()) == '*')
                b++;
            else
                unput(c);
        }
    }

    if (b != 0) {
        printf("\nUnclosed comment starting at line %d\n", yylineno);
        return -1;
    }
    addToken(&head, &tail, yylineno, MCOMMENT, yytext);
}

{INVALID_ID} { printf("Error on line %d: Identifiers cannot start with a number.\n", yylineno); }

{UNKNOWN} { printf("Error on line %d: Unrecognized character\n", yylineno); return(-1);}

<<EOF>> { printf("\nTotal lines: %d\nA re stephanie...\n\n", yylineno); return 1; }
    
%%

void
replace(char *str, char *from, char to)
{
    char *p = str;
    *p = to;
    memcpy((char *) p+1, (char *) p+2, strlen(p+1));
}

void
addToken(alpha_token_t **head, alpha_token_t **tail, unsigned int line, token_type_t type, void *content)
{
    alpha_token_t *new_token = malloc(sizeof(alpha_token_t));

    if (new_token == NULL)
        return;

    new_token->line = line;
    new_token->type = type;

    /* na valw elegxous gia fail ths malloc */
    switch (type) {
        case INTEGER:
            new_token->content = (int *) malloc(sizeof(int));
            *(int *)(new_token->content) = *(int *) content;
            break;
        case REAL:
            new_token->content = (double *) malloc(sizeof(double));
            *(double *)(new_token->content) = *(double *) content;
            break;
        default:
            new_token->content = (char *) malloc(sizeof(*content));
            strcpy(new_token->content, content);
    }

    if (*head == NULL) {
        *head = new_token;
        (*head)->next = NULL;
        *tail = *head;
        return;
    }

    (*tail)->next = new_token;
    *tail = new_token;
}

/* nystazw */
void
printTokens(alpha_token_t *head)
{
    unsigned int c = 1;

    while (head) {
        printf("%u:\t#%u\t", head->line, c++);
        switch (head->type) {
            case INTEGER:
                printf("\"%d\"", *(int *) head->content);
                break;
            case REAL:
                printf("\"%lf\"", *(double *) head->content);
                break;
            case STRING:
                printf("%s", (char *) head->content);
                break;
            default:
                printf("\"%s\"", (char *) head->content);
        }
        printf("\t%s\n", token_type_str[head->type]);
        head = head->next;
    }
}

int
main(int argc, char *argv[])
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

/* zZzZzZzZ */
