#include "../env_memory.h"

/* Executes the ASSIGN instruction */
void execute_assign(instr_s * instr);
void avm_assign(avm_memcell * lv, avm_memcell * rv,unsigned int line);