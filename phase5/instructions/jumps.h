#ifndef ENV_MEM_LIB
#include "../env_memory.h"
#endif

#ifndef DISPATCHER
#include "../dispatcher.h"
#endif

typedef unsigned char (*tobool_func_t)(avm_memcell *);
extern tobool_func_t to_bool_funcs[];

typedef unsigned char (*cmp_func)(double,double);
extern cmp_func cmp_functions[];

/* The functions used to perform the comparison */
unsigned char is_greater(double, double);
unsigned char is_greatereq(double,double);
unsigned char is_less(double,double);
unsigned char is_less_eq(double,double);

/* The functions which convert each type a boolean */
unsigned char double_tobool(avm_memcell *);
unsigned char int_tobool(avm_memcell *);
unsigned char string_tobool(avm_memcell *);
unsigned char bool_tobool(avm_memcell *);
unsigned char table_tobool(avm_memcell *);
unsigned char userfunc_tobool(avm_memcell *);
unsigned char libfunc_tobool(avm_memcell *);
unsigned char nil_tobool(avm_memcell *);
unsigned char undef_tobool(avm_memcell *);
unsigned char avm_tobool(avm_memcell *m);

/* Performs the jump (not) equal instruction */
void execute_jeq (instr_s * instr);

/* Performs the jump instruction */
void execute_jump(instr_s *);

/* Checks if the memcell represents a number value */
char is_num_type(avm_memcell_t type);

/* Performs the if_cmp instruction */
void execute_cmp(instr_s * instr);