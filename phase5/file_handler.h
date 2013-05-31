#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <assert.h>

#define MAGIC_NUMBER 2001993
#define EXPAND_SIZE 1024

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

/* The array that will include the final
   target instructions. */
extern instr_s * instructions;
extern unsigned int current_instr_index;
extern unsigned int total_instructions;

void read_binary_file(char *);
void read_magic_number(FILE *);
void read_arrays(FILE *);
void error_message(char *);
unsigned int add_const_to_array(void *,const_t);
void expand_const_array(const_t);
int memerror(void *, const char *);
void read_strings(FILE *);
void read_integers(FILE *);
void read_doubles(FILE *);
void read_user_functions(FILE *);
void read_lib_functions(FILE *);
void read_instructions(FILE *);
unsigned int get_instr_arg_num(vmopcode_e op);

vmarg_s * create_vmarg(void);
 
instr_s * create_instr(void);

char * vm_opcode_to_str(vmopcode_e op);
void print_instructions();
char * value_type_to_str(vmarg_t type);