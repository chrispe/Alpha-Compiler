#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "symbol_table.h"


/* =======================================================
	Struct and functions for storing strings in a stack 
   ======================================================= */

// A stack data stracture for
// storing temporary strings of functions.
typedef struct str_stack_node{
	char * str;
	struct str_stack_node * next;
}str_stack_node;

// The function for pushing elements in the stack.
void push(str_stack_node **,const char *);

// The function for popping elemens from the stack.
void pop(str_stack_node **);

// The function for getting the top of the stack.
char * top(str_stack_node *);

/* ==================================================
	Some global variables used for the right parsing 
   ================================================== */

// This is the main scope.
extern unsigned int scope_main;

// This is a scope for how deep the loop is.
extern unsigned int scope_loop;

// A boolean to know if we're in a function.
extern char in_func;

// An integer counting the scope at a function.
extern unsigned int func_scope;

// A number which indicates how many function we have set by the prefix '$_(id)'
extern unsigned int func_signed;

// A boolean which indicates if a function name has been called.
extern char func_started;

// A temporary string for a lot of uses.
extern char * temp_str;

// A variable which indicates if we recognized a function symbol.
extern char fun_rec;

// A variable which indicates if we reading the parameters of a function.
extern char func_var;

// A temporary pointer to the argument stack of a function.
extern arg_node * arg_tmp;

// A boolean which indicates if an expression after equal has started.
extern char expr_started;

// A stack which we push every time we visit a function
// and pop each time we leave a function.
extern str_stack_node * func_names;

/* ===========================================================
	The functions for adding variables to the symbol table 
   =========================================================== */

// Adds a variable to the symbol table performing every required checking.
void add_variable(symbol_table ** st, char * variable,unsigned int yylineno);

// Adds a local variable to the symbol table performing every required checking.
void add_local_variable(symbol_table ** st, char * variable,unsigned int yylineno);

// It just checks if the global reference is valid.
void check_global_variable(symbol_table ** st, char * variable,unsigned int yylineno);


/* ======================================================================
	The general function for inserting <functions> to the symbol table 
   ====================================================================== */
void add_function(symbol_table ** st, char * function,unsigned int yylineno,const char has_name);


/* =====================================================
	The general function for adding an argument to 
	the symbol table and to the function symbol.
   ===================================================== */
void add_function_argument(symbol_table ** st, char * argument,unsigned int yylineno,const char comma);


/* =====================================================
	General Functions
   ===================================================== */

// A function which returns the modulo of two doubles,
// so as no floating point exception occures.
double modulo(double a, double b);