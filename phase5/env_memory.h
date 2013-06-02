/* This library is for the functions of the enviroment's
   memory  (stack and constant arrays) */

#include "memory_manager.h"

#define ENV_MEM_LIB

#define AVM_STACK_ENV_SIZE 4

/* The main registers of the AVM */
extern avm_memcell ax, bx, cx;
extern avm_memcell retval;

/* The stack pointers */
extern unsigned int top, topsp;

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