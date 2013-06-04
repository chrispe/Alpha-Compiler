#ifndef ENV_MEM_LIB
#include "../env_memory.h"
#endif 

#ifndef DISPATCHER
#include "../dispatcher.h"
#endif
 
void execute_newtable(instr_s *);
void execute_tablegetelem(instr_s * instr); 
void execute_tablesetelem(instr_s * instr);

void avm_tablesetelem(avm_table ** table,avm_memcell * index,avm_memcell * data); 
avm_memcell * avm_tablegetitem(avm_table * table,avm_memcell * index);

avm_table_bucket * avm_lookuptable_bynumber(avm_table * table,avm_memcell * index);
avm_table_bucket * avm_lookuptable_bystring(avm_table * table,const char * index);
unsigned int generate_key(const char * name);


 
 
 