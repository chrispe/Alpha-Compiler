#include "parser_lib.h"

/* This enum is used to represent what kind
   of command is saved under the quad.   */
typedef enum iopcode {
	assign, add, sub, mul, op_div, mod,
	uminus, and, or, not,
	if_eq, if_neq, if_leq,
	if_greq, if_less, if_greater,
	call, param, ret, get_ret_val,
	func_start, func_end, jump, table_create,
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
typedef struct expr_s {
	expr_t type;
	st_entry * sym;
	struct expr_s * index;
	double num_value;
	char * str_value;
	unsigned char bool_value;
	struct expr_s * next;
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

typedef struct MethodCallParam{
	expr * elist;
	char method;
	char * name;
}method_call_param;

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

/* A 'special' emit function for the
   management of the table items. */
expr * emit_iftableitem(expr *,symbol_table **,unsigned int);

/* Sets the label of a quad in the array. */
void patch_label(unsigned int, unsigned int);

/* This function prints the quads to a text file. */
void write_quads(void);

/* Creates an expression based on a symbol. */
expr * lvalue_expr(st_entry *);

/* Creates a new expression based on expression type argument. */
expr * new_expr(expr_t);

/* Creates a new expression for a string value. */
expr * new_expr_const_str(char *);

/* Creates a new member item expression */
expr * new_member_item_expr(expr *,char *,symbol_table **,unsigned int);

/* Returns the opcode to string */
char * opcode_to_str(opcode);

/* For the function call */
expr * make_call(expr *,expr *,symbol_table **,unsigned int);
 