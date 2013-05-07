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
