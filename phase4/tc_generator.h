#include "ic_generator.h"

// The defined values for expanding the constants arrays and the instruction table.
#define DOUBLE_ARR_SIZE total_double_consts*sizeof(double)
#define INTEGER_ARR_SIZE total_integer_consts*sizeof(int)
#define STRING_ARR_SIZE total_str_consts*sizeof(char *)
#define USER_FUNC_ARR_SIZE total_user_funcs*sizeof(userfunc_s)
#define LIB_FUNC_ARR_SIZE total_named_lib_funcs*sizeof(char *)
#define INSTR_ARR_SIZE total_instructions*sizeof(instr_s)
 
#define DOUBLE_ARR_NEW_SIZE EXPAND_SIZE*sizeof(double) + DOUBLE_ARR_SIZE
#define INTGER_ARR_NEW_SIZE EXPAND_SIZE*sizeof(int) + INTEGER_ARR_SIZE
#define	STRING_ARR_NEW_SIZE EXPAND_SIZE*sizeof(char *) + STRING_ARR_SIZE
#define	USER_FUNC_ARR_NEW_SIZE EXPAND_SIZE*sizeof(userfunc_s) + USER_FUNC_ARR_SIZE
#define	LIB_FUNC_ARR_NEW_SIZE EXPAND_SIZE*sizeof(char *) + LIB_FUNC_ARR_SIZE
#define INSTR_ARR_NEW_SIZE EXPAND_SIZE*sizeof(instr_s) + INSTR_ARR_SIZE

typedef void (* generator_func_t)(quad *);

/* The types of a VM opcode */
typedef enum vmopcode{
	assign_v=0, add_v=1, sub_v=2, mul_v=3, div_v=4, mod_v=5,
	uminus_v=6, and_v=7, or_v=8, not_v=9, jeq_v=10, jne_v=11,
	jle_v=12, jge_v=13, jlt_v=14, jgt_v=15, call_v=16, pusharg_v=17,
	ret_v=18, getretval_v=19, funcenter_v=20, funcexit_v=21,
	jump_v=22, newtable_v=23, tablegetelem_v=24, tablesetelem_v=25,
	nop_v = 26
}vmopcode_e;

/* The types of a VM argument */
typedef enum vmargtype{
	label_a = 0,
	global_a = 1,
	formal_a = 2,
	local_a = 3,
	integer_a = 4,
	double_a = 5,
	string_a = 6,
	bool_a = 7,
	nil_a = 8,
	userfunc_a = 9,
	libfunc_a = 10,
	retval_a = 11
}vmarg_t;

/* The structure of a VM argument */
typedef struct vmarg {
	vmarg_t type;
	unsigned int value;
}vmarg_s;

/* The structure of a VM instruction */
typedef struct instruction {
	vmopcode_e opcode;
	vmarg_s * result;
	vmarg_s * arg1;
	vmarg_s * arg2;
	unsigned int line; 
}instr_s;

/* The structure of a defined user function */
typedef struct userfunc {
	unsigned int address;
	unsigned int local_size;
	char * name;
}userfunc_s;

/* The types of a constant */
typedef enum const_type{
	double_c,
	int_c,
	str_c,
	user_func_c,
	lib_func_c
}const_t;

typedef struct funcstack{
	st_entry * sym;
	list_node * ret_list;
	unsigned int line;
	struct funcstack * next;
}func_stack;


/* The struct of an incomplete jump
   which we will complete after the
   target code has been generated. */
typedef struct incompletejump{
	unsigned int instr_id;
	unsigned int iaddress;
	struct incompletejump * next;
}incomplete_jump;

/* The expandable arrays for the constants of the code */

// For the double number constants 
extern double * double_consts;
extern unsigned int current_double_index;
extern unsigned int total_double_consts;

// For the integer number constants
extern int * integer_consts;
extern unsigned int current_int_index;
extern unsigned int total_integer_consts;

// For the strings
extern char ** str_consts;
extern unsigned int current_str_index;
extern unsigned int total_str_consts;

// For the library functions
extern char ** named_lib_funcs;
extern unsigned int current_lib_func_index;
extern unsigned int total_named_lib_funcs;

// For the user functions
extern userfunc_s * user_funcs;
extern unsigned int current_user_func_index;
extern unsigned int total_user_funcs;

/* The list for the incomplete jumps */
extern incomplete_jump * i_jumps;
extern unsigned int i_jumps_total;

/* The array that will include the final
   target instructions. */
extern instr_s * instructions;
extern unsigned int current_instr_index;
extern unsigned int total_instructions;

/* The stack used to save functions
   during the target code generation. */
extern func_stack  * funcs;

/* Adds an incomplete jump to the i_jump list */
void add_incomplete_jump(unsigned int, unsigned int);

/* For addding a new constant to the right array, returns the index to it.*/
unsigned int add_const_to_array(void *,const_t);

/* For expanding the constants array in case it is full */
void expand_const_array(const_t);

/* Checks if the value exists in the array 
   Returns -1 if not else returns the index to it */
unsigned int value_exists_in_arr(void *, const_t);

/* Creating a new vm_arg */
vmarg_s * create_vmarg(void);

/* Resets the operand (aka vmarg) */
void reset_operand(vmarg_s *);

/* Creating a new instr_s */
instr_s * create_instr(void);

/* The instruction memory destoyer */
void destory_instr(instr_s *);

/* Creates an operand based on the expression */
vmarg_s * make_operand(expr * e, vmarg_s *);

/* Creates a vmarg based on a double */
void make_double_operand(vmarg_s *,double *);

/* Creates a vmarg based on an integer */
void make_integer_operand(vmarg_s *,int);

/* Creates a vmarg based on a boolean */
void make_boolean_operand(vmarg_s *,unsigned int);

/* Creates a vmarg based on a return value */
void make_retval_operand(vmarg_s *);

/* Emits an instruction to the instructions array */
void emit_instruction(instr_s *);

/* Expands the instruction array in case it is full */
void expand_instr_array(void);

/* Patches the incompleted jump instructions */
void patch_incomplete_jumps(void);

/* Generates the instruction based on the opcode and a quad */
void generate(vmopcode_e, quad *);

/* Returns the next instruction label */
unsigned int next_instr_label(void);

/* Generates the instructions */
void generate_instructions(void);

/* The generator for each opcode */ 
extern void generate_ADD(quad *);
extern void generate_SUB(quad *);
extern void generate_MUL(quad *);
extern void generate_DIV(quad *);
extern void generate_MOD(quad *);
extern void generate_UMINUS(quad *);
extern void generate_NEWTABLE(quad *); 
extern void generate_TABLEGETELEM(quad *); 
extern void generate_TABLESETELEM(quad *); 
extern void generate_ASSIGN(quad *);
extern void generate_NOP(quad *);
extern void generate_NOT(quad *);
extern void generate_AND(quad *);
extern void generate_OR(quad *);

extern void generate_relational(vmopcode_e, quad *);
extern void generate_JUMP(quad *);
extern void generate_IF_EQ(quad *);
extern void generate_IF_NOTEQ(quad *);
extern void generate_IF_GREATER(quad *);
extern void generate_IF_GREATEREQ(quad *);
extern void generate_IF_LESS(quad *);
extern void generate_IF_LESSEQ(quad *);

extern void generate_PARAM(quad *);
extern void generate_CALL(quad *);
extern void generate_GETRETVAL(quad *);
extern void generate_FUNCSTART(quad *);
extern void generate_FUNCEND(quad *);
extern void generate_RETURN(quad *);

/* Prints the string array */
void print_string();

/* Prints the user functions array */
void printFun();

/* All the function stack operations */
void push_func(func_stack ** top,st_entry * sym, unsigned int line);
func_stack * top_func(func_stack * top);
void pop_func(func_stack ** top);

/* Returns the opcode of a vmarg as a string */
char * vm_opcode_to_str(vmopcode_e op);

/* Returns the type of a value as a string */
char * value_type_to_str(vmarg_t);

void write_arrays(FILE * output);

void write_magic_number(FILE * output);

void write_code(FILE * output);