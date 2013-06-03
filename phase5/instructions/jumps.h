#ifndef ENV_MEM_LIB
#include "../env_memory.h"
#endif

#ifndef DISPATCHER
#include "../dispatcher.h"
#endif

typedef unsigned char (*tobool_func_t)(avm_memcell *);
extern tobool_func_t to_bool_funcs[];

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

void execute_jump(instr_s *);