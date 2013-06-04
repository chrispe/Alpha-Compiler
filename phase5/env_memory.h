/* This library is for the functions of the enviroment's
   memory  (stack and constant arrays) */

#ifndef FILE_HANDLER
#include "file_handler.h"
#endif
  
#define ENV_MEM_LIB
#define AVM_STACK_ENV_SIZE 4
#define AVM_STACKSIZE 4096
#define AVM_WIPEOUT(m) memset(&(m),0,sizeof(avm_memcell))
#define AVM_TABLE_HASHSIZE 211

/* The type of data that a memory cell can represent */
typedef enum avm_memcell_type{
	double_m = 0,
	integer_m = 1,
	string_m = 2,
	bool_m = 3,
	table_m = 4,
	userfunc_m = 5,
	libfunc_m = 6,
	nil_m = 7,
	undefined_m = 8
}avm_memcell_t;

/* The structure of a memory cell in the AVM */
typedef struct avm_memory_cell{
	avm_memcell_t type;
	union {
		double double_value;
		int int_value;
		char * str_value;
		unsigned char bool_value;
		unsigned int func_value;
		char * lib_func_value;
		struct avm_table * table_value;
	}data;
}avm_memcell;

/* The stucture of a bucker for the table */
typedef struct avm_table_bucket{
	avm_memcell * key;
	avm_memcell * value;
	struct avm_table_bucket * next;
}avm_table_bucket;

/* The structure used to create tables */
typedef struct avm_table{
	unsigned int ref_counter;
	avm_table_bucket * str_indexed[AVM_TABLE_HASHSIZE];
	avm_table_bucket * num_indexed[AVM_TABLE_HASHSIZE];
	unsigned int total;
}avm_table;

/* The memory stack of the AVM */
extern avm_memcell stack[AVM_STACKSIZE];

/* The main registers of the AVM */
extern avm_memcell ax, bx, cx;
extern avm_memcell retval;

/* The stack pointers */
extern unsigned int top, topsp;

/* The number of arguments under the caller */
extern unsigned int total_actuals;

/* Initialzes all the memory cells to undefined */
void avm_init_stack(void);

/* All the functions that can be used on a table */
avm_table * avm_newtable(void);
 
void avm_settableitem(avm_memcell *,avm_memcell *);
void avm_destorytable(avm_table *);
void avm_table_incr_refcounter(avm_table *);
void avm_table_decr_refcounter(avm_table *);
void avm_table_bucket_init(avm_table_bucket **);
void avm_table_bucket_destroy(avm_table_bucket **);

/* Erases completely a memory cell */
void avm_clear_memcell(avm_memcell *);

typedef char * (*tostring_func_t)(avm_memcell *);

/* Converts the content of a memcell to string */
char * avm_tostring(avm_memcell *);

extern char * double_tostring (avm_memcell *);
extern char * int_tostring (avm_memcell *);
extern char * string_tostring (avm_memcell *);
extern char * bool_tostring (avm_memcell *);
extern char * table_tostring (avm_memcell *);
extern char * userfunc_tostring (avm_memcell *);
extern char * libfunc_tostring (avm_memcell *);
extern char * nil_tostring (avm_memcell *);
extern char * undef_tostring (avm_memcell *);

extern tostring_func_t to_str_funcs[];

/* A string constructor with a length parameter */
char * create_string(unsigned int len);

/* A avm_memcell contructor */
avm_memcell * create_memcell();

/* Returns the type of data that the
   memcell represents to a string */
char * value_type_to_str(avm_memcell_t);
char * arg_value_type_to_str(vmarg_t type);

/* The functions that retrieve values
   from the constants arrays */
double consts_getdouble(unsigned int);
int consts_getint(unsigned int);
char * libfuncs_getused(unsigned int);
char * consts_getstr(unsigned int);
userfunc_s * userfuncs_getfunc(unsigned int);

/* Returns the memory cell that is 
   referenced in an instruction */
avm_memcell * avm_translate_operand(vmarg_s *,avm_memcell *);

/* Displays a warning message */
void avm_warning(char *,char *,char *, unsigned int);

/* Displays an error message and exits the AVM */
void avm_error(char *, char *, char *,unsigned int);

/* Displays an error message (without line) and exits the AVM */
void avm_anonymous_error(char *);

extern unsigned int current_func_size;