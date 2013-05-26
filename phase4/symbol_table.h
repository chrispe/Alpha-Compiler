/*
	The ADT used for the Symbol Table Implementation.
*/

#define BUCKET_SIZE 1000
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// The type of the variable/function.
typedef enum{
	GLOBAL_VAR, VAR, LCAL, FORMAL,
	USERFUNC, LIBFUNC, TEMP_VAR
}st_entry_type;

typedef enum{
	PROGRAM_VAR,
	FUNC_LOCAL,
	FORMAL_ARG
}scopespace_t;

// A linked list used to save the arguments of a function.
typedef struct ArgNode{
	char * name;
	struct ArgNode * next;
}arg_node;

// The struct for the symbol type of a variable.
typedef struct Variable {
	char * used_in_func;
}variable;

// The struct for the symbol type of a function.
typedef struct Function{
	arg_node * arguments;
}function;

// The structure of a symbol table entry.
typedef struct SymbolTableEntry{
	char * name;
	unsigned int active;
	unsigned int scope;
	unsigned int line;
	unsigned int offset;
	unsigned int taddress;
	scopespace_t space;
	st_entry_type type;
	union{
		variable * varVal;
		function * funVal;
	}value_type;
	struct SymbolTableEntry * next;
}st_entry;

// A scope entry.
typedef struct ScopeEntry{
	unsigned int scope;
	st_entry * symbols;
	struct ScopeEntry * next;
}scope_entry;

/*
	The main structure of the symbol table.
	Contains a hash table so we can store and retrieve
	any symbol from it. But, we also have a scope list.
	In each scope node we insert the symbols that have the same scope.
*/
typedef struct Symbol_Table{
	st_entry * hash_table[BUCKET_SIZE];
	scope_entry * scope_list;
	st_entry * last_symbol;
}symbol_table;

/* 
	Creates a symbol table and returns a pointer to it.
	Warning : The returned symbol table has already the default library functions.
*/
symbol_table * create_symbol_table(void);

/*	
	A general fuction for inserting a new symbol to the symbol table.
	Returns 1 if insertion was a succecss else returns 0. 
*/
int st_insert(symbol_table ** st, st_entry ** symbol);

// A general function for looking up a symbol under the symbol table using the hash table.
st_entry * st_lookup_table(symbol_table * st,const char * symbol_name);

// A general function for looking up a symbol under the symbol table using scope list.
st_entry * st_lookup_scope(symbol_table * st,const char * symbol_name,unsigned int scope);

// The hash function (generates key by giving the symbol name) for the hash table of the symbol table.
int generate_key(const char * name);

// Sets the scope variables either hidden or visible.
void scope_set_active(symbol_table ** st,unsigned int scope,const char active);

// Adds a new arg to the argument list.
int args_insert(arg_node ** args,const char * arg_name);

// Returns the node with that argument.
arg_node * args_lookup(arg_node * args,const char * arg_name);

// Prints the symbol table.
void print_st(symbol_table * st);

// Prints an error in case the memory allocation failed.
int memerror(void * ptr, const char * name);

// Sets the function in which the symbol (var) was declared.
st_entry * set_var_func(st_entry * symbol,const char * func_name);

// Creates a symbol with the given parameters (name,active,scope,line,type)
st_entry * create_symbol(const char *, unsigned int, unsigned int,unsigned int,st_entry_type,unsigned int offset, scopespace_t space);

// Counts the arguments of a function symbol.
unsigned int count_func_args(st_entry *);