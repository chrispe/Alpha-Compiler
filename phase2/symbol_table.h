/*
	The ADT used for the Symbol Table Implementation.
*/

#define BUCKET_SIZE 1000

/*
	The main structure of the symbol table.
	Contains a hash table so we can store and retrieve
	any symbol from it. But, we also have a scope list.
	In each scope node we insert the symbols that have the same scope.
*/

typedef struct Symbol_Table{
	st_entry * hash_table[BUCKET_SIZE];
	scope_entry * scope_list;
}symbol_table;

// The structure of a symbol table entry.
typedef struct SymbolTableEntry{
	char * name;
	unsigned int active;
	unsigned int scope;
	unsigned int line;
	sb_entry_type type;
	union{
		variable * varVal;
		function * funVal;
	}value_type;
	struct SymbolTableEntry * next;
}st_entry;

// The type of the variable/function.
typedef enum{
	GLOBAL, LOCAL, FORMAL,
	USERFUNC, LIBFUNC
}st_entry_type;

// The struct for the symbol type of a variable.
typedef struct Variable {
	char * used_in_func;
}variable;

// The struct for the symbol type of a function.
typedef struct Function{
	arg_node * arguments;
}function;

// A linked list used to save the arguments of a function.
typedef struct ArgNode{
	char * arg_name;
	struct ArgNode * next;
}arg_node;

// A scope entry.
typedef struct ScopeEntry{
	unsigned int scope;
	st_entry * symbol;
	struct ScopeEntry * next;
}scope_entry;

/* 
	Creates a symbol table and returns a pointer to it.
	Warning : The returned symbol table has already the default library functions.
*/
symbol_table * create_symbol_table();

/*	
	A general fuction for inserting a new symbol to the symbol table.
	Returns 1 if insertion was a succecss else returns 0. 
*/
int st_insert(symbol_table ** st, st_entry symbol);

// A general function for looking up a symbol under the symbol table using the hash table.
st_entry * st_lookup_table(symbol_table * st,const char * symbol_name);

// A general function for looking up a symbol under the symbol table using scope list.
st_entry * st_lookup_list(symbol_table * st,const char * symbol_name);

// The hash function (generates key by giving the symbol name) for the hash table of the symbol table.
int generate_key(const char * name);

// Sets the symbol either hidden or visible depending on the <hidden> variable.
void symbol_set_hidden(st_entry ** symbol,const char hidden);

// Sets the block variables either hidden or visible.
void block_set_hidden(st_entry ** symbol,const char * func,const char hidden);

// Adds a new arg to the argument list.
void args_insert(arg_stack ** args,const char * arg_name);

// Returns the node with that argument.
arg_stack * args_lookup(arg_stack ** args,const char * arg_name);

// Prints the symbol table.
void print_st(symbol_table * st);