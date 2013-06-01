/* This library implements the memory functions
   for the alpha virtual machine (aka AVM)  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"

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

/* Initialzes all the memory cells to undefined */
static void avm_init_stack(void);

/* All the functions that can be used on a table */
avm_table * avm_newtable(void);
avm_memcell * avm_tablegetitem(avm_memcell *);
void avm_settableitem(avm_memcell *,avm_memcell *);
void avm_destorytable(avm_table *);
void avm_table_incr_refcounter(avm_table *);
void avm_table_decr_refcounter(avm_table *);
void avm_table_bucket_init(avm_table_bucket **);
void avm_table_bucket_destroy(avm_table_bucket **);

/* Erases completely a memory cell */
void avm_clear_memcell(avm_memcell *);