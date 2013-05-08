#include "parser_lib.h"

/* This enum is used to represent what kind
   of command is saved under the quad.   */
typedef enum iopcode {
	assign, add, sub, mul, op_div, mod,
	uminus, and, or, not,
	if_eq, if_neq, if_leq,
	if_greq, if_less, if_greater,
	call, param, ret, get_ret_val,
	func_start, func_end, table_create,
	table_get_elem, table_set_elem
}opcode;

/* This enum is used to represent what
   kind of expression is saved under the expr type */
typedef enum expression_type{
	var_e,
	table_item_e,
	program_func_e,
	library_func_e,
	arithm_expr_e,
	bool_expr_e,
	assign_expr_e,
	new_table_e,
	const_num_e,
	const_bool_e,
	const_str_e,
	null_e
}expr_t;

/* This is the structure of a stored expression. */
typedef struct Expr {
	expr_t type;
	st_entry * s;
	struct Expr * index;
	double num_value;
	char * str_value;
	unsigned char bool_value;
	struct Expr * next;
}expr;

/* This is the structure of the quad. */
typedef struct Quad {
	opcode op;
	expr * result;
	expr * arg1;
	expr * arg2;
	unsigned int label;
	unsigned int line;
}quad;

/* The array of the quads. */
extern quad * quads;

/* The toal number of quads. */
extern unsigned int quads_total;

/* The current index on the array of quads. */
extern unsigned int curr_quad;

/* Some useful defined keywords for the
   reallocation of the quads array.	 */
#define EXPAND_SIZE 1024
#define CURR_SIZE (quads_total*sizeof(quad))
#define NEW_SIZE (EXPAND_SIZE*sizeof(quad) + CURR_SIZE)

/* This function is used to expand the quads array */
void expand(void);

/* This function is for the insertion
   of a new quad to the quads array. */
void emit(opcode,expr *,expr *, expr *, unsigned int,unsigned int);

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
 
/* Increases the current scope offset */
void increase_curr_scope_offset(void);

/* Increases the scope space counter 
   We use this when we enter a scope space */
void enter_scope_space(void);

/* Decreases the scope space counter
   We use this when we exit a scope space */
void exit_scope_space(void);