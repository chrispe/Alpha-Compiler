#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "symbol_table.h"

/* =======================================================
	Struct and functions for storing strings in a stack 
   ======================================================= */

// A stack data stracture for
// storing temporary strings/values of functions.
typedef struct str_stack_node{
	char * str;
	unsigned int  value;
	struct str_stack_node * next;
}str_stack_node;

// The function for pushing elements in the stack.
void push(str_stack_node **,const char *);
void push_value(str_stack_node ** top,unsigned int  val);

// The function for getting the top of the stack.
char * top(str_stack_node *);
unsigned int top_value(str_stack_node * top);

// The function for popping elemens from the stack.
void pop(str_stack_node **);

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

// A number which indicates how many function we have set by the prefix '$f_(id)'
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

// A stack which we push every time we visit a function.
// and pop each time we leave a function.
extern str_stack_node * func_names;

// A stack which keeps the loop scope when entering a function.
extern str_stack_node * loop_stack;

// A stack which keeps the scope offset for each function.
extern str_stack_node * scope_offset_stack;

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
	Some general procedures for adding functions to a symbol.
   ====================================================================== */

void add_function(symbol_table ** st, char * function,unsigned int yylineno,const char has_name);

// Generates a special function name with the prefix "$_f[id]" where id the argument
char * generate_func_name(unsigned int id);

/* =====================================================
	The general function for adding an argument to 
	the symbol table and to the function symbol.
   ===================================================== */
void add_function_argument(symbol_table ** st, char * argument,unsigned int yylineno,const char comma);


/* ======================================================
	Some variables and functions for the symbol scoping
   ====================================================== */

/* Generates a name for a temporary variable */
char * generate_temp_var(unsigned int id);

/* A number which indicates how many temporary
   variables we have set by the prefix '$v_(id)' */
extern unsigned int var_signed;

/* Resets the counter of the temporary variables. */
void reset_temp_vars(void);

/* Returns the current scope */
unsigned int get_current_scope(void);

/* Returns a symbol to be used for a temporary variable */
st_entry * get_temp_var(symbol_table * st, unsigned int line);

/* Variables used to get the offset of each kind of symbol */
extern unsigned int program_var_offset;
extern unsigned int func_local_offset;
extern unsigned int formal_arg_offset;

/* A counter for the scope space */
extern unsigned int scope_space_counter;

/* Returns the current scope space */
scopespace_t get_current_scope_space(void);

/* Returns the current scope offset */
unsigned int get_current_scope_offset(void);
 
/* Sets the current scope to a fixed offset.*/
void set_curr_scope_offset(unsigned int);

/* Increases the current scope offset */
void increase_curr_scope_offset(void);

/* Increases the scope space counter 
   We use this when we enter a scope space */
void enter_scope_space(void);

/* Decreases the scope space counter
   We use this when we exit a scope space */
void exit_scope_space(void);

/* =====================================================
	General Functions
   ===================================================== */

// A function which returns the modulo of two doubles,
// so as no floating point exception occures.
double modulo(double a, double b);