#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <assert.h>

#define MAGIC_NUMBER 2001993

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
	char * name;
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

/* The arrays for the constants of the code */

// For the double number constants 
extern double * double_consts;
extern unsigned int total_double_consts;

// For the integer number constants
extern int * integer_consts;
extern unsigned int total_integer_consts;

// For the strings
extern char ** str_consts;
extern unsigned int total_str_consts;

// For the library functions
extern char ** named_lib_funcs;
extern unsigned int total_named_lib_funcs;

// For the user functions
extern userfunc_s * user_funcs;
extern unsigned int total_user_funcs;

/* The array that will include the final
   target instructions. */
extern instr_s * instructions;
extern unsigned int total_instructions;

/* The functions used to read the binary
   file and initiliaze the arrays */
void read_binary_file(char *);
void read_magic_number(FILE *);
void read_arrays(FILE *);
void read_strings(FILE *);
void read_integers(FILE *);
void read_doubles(FILE *);
void read_user_functions(FILE *);
void read_lib_functions(FILE *);
void read_instructions(FILE *);

/* Displays an error message and exits */
void error_message(char *);

/* Detects if any memory allocation failed
   in case of yes, it displays a message and exits */
void memerror(void *, const char *);
 
 /* Returns how many arguments an instruction with that op has */
unsigned int get_instr_arg_num(vmopcode_e op);

/* Creates a new vmarg */
vmarg_s * create_vmarg(void);