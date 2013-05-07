/*
    A library used for the lexical analyzer.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char *token_type_str[];
extern char *key_str[];
extern char *op_str[];
extern char *pun_str[];
 
        
// Extends the string by a given char
char * extendString(char *,char);

// Replaces a char in a string with another char
void replace(char *, char *, char);

// Returns the hex value of a string representing a hex value
int htoi(char *);