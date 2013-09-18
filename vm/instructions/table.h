/* Includes the functions that are used
   to execute the table instructions */

#ifndef ENV_MEM_LIB
#include "../env_memory.h"
#endif 

#ifndef DISPATCHER
#include "../dispatcher.h"
#endif
 
/* Performs the create table instruction */ 
void execute_newtable(instr_s *);

/* Performs the table get element instruction */
void execute_tablegetelem(instr_s * instr);

/* Performs the table set element instruction */
void execute_tablesetelem(instr_s * instr);

/* Sets an element of the table */
void avm_tablesetelem(avm_table ** table,avm_memcell * index,avm_memcell * data); 

/* Gets an element of the table */
avm_memcell * avm_tablegetitem(avm_table * table,avm_memcell * index);

/* Looks up on the hash table of the table that has a number as an index */
avm_table_bucket * avm_lookuptable_bynumber(avm_table * table,avm_memcell * index);

/* Looks up on the hash table of the table that has a string as an index */ 
avm_table_bucket * avm_lookuptable_bystring(avm_table * table,const char * index);

/* Generates a key based on a string */
unsigned int generate_key(const char * name);