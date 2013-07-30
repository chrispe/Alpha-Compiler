#include "lex_lib.h"

char *key_str[] = {
    "if", "else", "while", "for", "function",
    "return", "break", "continue", "and", "not",
    "or", "local", "true", "false", "nil"
};

char *op_str[] = {
    "=", "+", "-", "*", "/", "%", "==", "!=",
    "++", "--", ">", "<", ">=", "<="
};

char *pun_str[] = {
    "{", "}", "[", "]", "(", ")", ";", ",", ":", "::", ".", ".."
};

int htoi(const char * p)
{
    if ((p[1] == 'x')  || (p[1] == 'X'))
        return(strtol(&p[2], (char **)0, 16));
    return(strtol(p, (char **)0, 16));
}

void replace(char *str, char *from, char to)
{
    char * p = str;
    *p = to;
    memmove((char *) p+1, (char *) p+2, strlen(p+1));
}