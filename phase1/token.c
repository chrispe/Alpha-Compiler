#include "token.h"

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

void replace(char *str, char *from, char to)
{
    char *p = str;
    *p = to;
    memcpy((char *) p+1, (char *) p+2, strlen(p+1));
}

void addToken(alpha_token_t **head, alpha_token_t **tail, unsigned int line, token_type_t type, void *content)
{
    alpha_token_t *new_token = malloc(sizeof(alpha_token_t));

    if (new_token == NULL)
        return;

    new_token->line = line;
    new_token->type = type;

    switch (type) {
        case INTEGER:
            new_token->content = (int *) malloc(sizeof(int));
            if(new_token->content==NULL)return;
            *(int *)(new_token->content) = *(int *) content;
            break;
        case REAL:
            new_token->content = (double *) malloc(sizeof(double));
            if(new_token->content==NULL)return;
            *(double *)(new_token->content) = *(double *) content;
            break;
        default:
            new_token->content = (char *) malloc(sizeof(*content));
            if(new_token->content==NULL)return;
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

void printTokens(alpha_token_t *head)
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