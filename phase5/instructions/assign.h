#ifndef ENV_MEM_LIB
#include "../env_memory.h"
#endif

#ifndef DISPATCHER
#include "../dispatcher.h"
#endif

/* Executes the ASSIGN instruction */
void execute_assign(instr_s * instr);

/* Performs the actual actions needed for the ASSIGN instruction */
void avm_assign(avm_memcell * lv, avm_memcell * rv,unsigned int line);