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
	const_int_e,
	const_num_e,
	const_bool_e,
	const_str_e,
	nil_e
}expr_t;

/* This is the structure of a stored expression. */
typedef struct expr_s {
	expr_t type;
	st_entry * sym;
	struct expr_s * index;
	int int_value;
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

/* An expression list for the index items */
extern expr * index_expr;
 

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

/* Creates an expression based on a constant double */ 
expr * new_expr_const_num(double); 

/* Creates an expression based on a constant integer */
expr * new_expr_const_int(int);

/* Creates an expression based on a constan boolean */
expr * new_expr_const_bool(unsigned int);

/* Converts an expression to a string */
char * expr_to_str(expr *);

/* Checks if uminus is used incorrectly and
   prints an error message in case it does. */
void check_uminus(expr *,unsigned int);

/* Prints a compile error message */
void comp_error(char * error,unsigned int line);

/* Returns if the name represents a temp variable */
unsigned int name_is_temp(char *);

/* Returns if the expression represents a temp symbol */
unsigned int expr_is_temp(expr *);

/* Returns if an expression is valid for arithmetic operation */
unsigned int arithm_expr_valid(expr *);

/* Returns if the expression represents a number (either double or int). */
unsigned int is_num_expr(expr *,unsigned int *);

/* Returns the value of an number expression */
double get_expr_num_value(expr * e);

/* Emits an arithmetic expression */
expr * emit_arithm(symbol_table ** st,opcode,expr *,expr *, expr *, unsigned int,unsigned int);

/* Applies the arithmetic operation based on the op argument. */
double apply_arithm_op(opcode,double,double,unsigned int);