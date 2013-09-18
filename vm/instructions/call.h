#ifndef ENV_MEM_LIB
#include "../env_memory.h"
#endif

#ifndef DISPATCHER
#include "../dispatcher.h"
#endif

#define AVM_NUMACTUALS_OFFSET 4
#define AVM_SAVEDPC_OFFSET 3
#define AVM_SAVEDTOP_OFFSET 2
#define AVM_SAVEDTOPSP_OFFSET 1

/* Executes the call instruction */
void execute_call(instr_s * instr);

/* Pushes an enviroment value to the stack */
void avm_push_envvalue(unsigned int);

/* Saves the enviroment values to the stack */
void avm_call_save_env(void);

/* Decreases the top by one */
void avm_dec_top(void);

/* Executes the funcenter instruction */
void execute_funcenter(instr_s * instr);

/* Returns the info of a user function based on the
   userfuncs constant array (by giving the index) */
userfunc_s * avm_get_func_info(unsigned int index);
 
/* Returns the enviroment value */ 
unsigned int avm_get_env_value(unsigned int);

/* Executes the funcexit instruction */ 
void execute_funcexit(instr_s *);

/* This is used when calling a library function */
void avm_call_libfunc(char *);

/* Returns the total actuals of the caller */
unsigned int avm_total_actuals(void);

/* Returns the i-th actual of the call */
avm_memcell * avm_get_actual(unsigned int);

/* Looks up if the library function 
   with argument string value exists */
unsigned char avm_library_func_exist(char *);

/* Executes the pusharg instruction */
void execute_pusharg(instr_s *);

/* Converts the contents of a memcell to a string */
char * avm_tostring(avm_memcell * m);

/* Prints the stack of the AVM */
void printstack();