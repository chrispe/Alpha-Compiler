/*
    This is an interface used for the
    functionality of tokens.
*/


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