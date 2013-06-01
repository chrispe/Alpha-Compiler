#include "memory_manager.h"

avm_memcell stack[AVM_STACKSIZE];

static void avm_init_stack(void){
	unsigned int i;
	for(i=0;i<AVM_STACKSIZE;i++){
		AVM_WIPEOUT(stack[i]);
		stack[i].type = undefined_m;
	}
}

void avm_table_bucket_init(avm_table_bucket ** bucket){
	unsigned int i;
	for(i=0;i<AVM_TABLE_HASHSIZE;i++){
		bucket[i] = (avm_table_bucket *) NULL;
	}
}

avm_table * avm_newtable(void){
	avm_table * table = (avm_table *)malloc(sizeof(avm_table));
	memerror(table,"new table");
	AVM_WIPEOUT(*table);

	table->ref_counter = 0;
	table->total = 0;
	avm_table_bucket_init(table->num_indexed);
	avm_table_bucket_init(table->str_indexed);
	return table;
}

void avm_destorytable(avm_table * table){
	avm_table_bucket_destroy(table->str_indexed);
	avm_table_bucket_destroy(table->num_indexed);
	free(table);
}

void avm_table_incr_refcounter(avm_table * table){
	table->ref_counter++;
}

void avm_table_decr_refcounter(avm_table * table){
	assert(table->ref_counter>0);
	table->ref_counter--;
	if(table->ref_counter==0)
		avm_destorytable(table);
}
 
void avm_clear_memcell(avm_memcell * cell){
	if(cell->type!=undefined_m){
		if(cell->type==string_m)
			free(cell->data.str_value);
		else if(cell->type==table_m)
			avm_table_decr_refcounter(cell->data.table_value);
	}
}

void avm_table_bucket_destroy(avm_table_bucket ** bucket){
	unsigned int i;
	avm_table_bucket * del_b;
	for(i=0;i<AVM_TABLE_HASHSIZE;i++){
		while(bucket[i]){
			del_b = bucket[i];
			bucket[i] = bucket[i]->next;
			avm_clear_memcell(del_b->value);
			avm_clear_memcell(del_b->key);
			free(del_b);
		}
		bucket[i] = NULL;
	}
}