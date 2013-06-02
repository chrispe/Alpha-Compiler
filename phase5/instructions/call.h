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

extern unsigned int total_actuals;

void execute_call(instr_s * instr);

void avm_push_envvalue(unsigned int);

void avm_call_save_env(void);

void avm_dec_top(void);

void execute_funcenter(instr_s * instr);

userfunc_s * avm_get_func_info(unsigned int index);
 
unsigned int avm_get_env_value(unsigned int);
 
void execute_funcexit(instr_s *);

void avm_call_libfunc(char *);

unsigned int avm_total_actuals(void);

avm_memcell * avm_get_actual(unsigned int);

unsigned char avm_library_func_exist(char *);

void execute_pusharg(instr_s *);

char * avm_tostring(avm_memcell * m);

void printstack();