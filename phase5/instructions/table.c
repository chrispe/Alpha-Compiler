#include "table.h"

void execute_newtable(instr_s * instr){
	avm_memcell * lv = avm_translate_operand(instr->result,(avm_memcell *)NULL);

	avm_clear_memcell(lv);
	lv->type = table_m;
	lv->data.table_value = avm_newtable();
	avm_table_incr_refcounter(lv->data.table_value);
}

void execute_tablegetelem(instr_s * instr){
	
	avm_memcell * lv = avm_translate_operand(instr->result,(avm_memcell *)NULL);
	avm_memcell * t = avm_translate_operand(instr->arg1,(avm_memcell *)NULL);
	avm_memcell * i = avm_translate_operand(instr->arg2,&ax);
 
	avm_clear_memcell(lv);
	lv->type = nil_m;

	if(t->type != table_m)
		avm_error("Illegal use of variable (",instr->arg1->name,") as a table",instr->line);
	else{
		avm_memcell * content = avm_tablegetitem(t->data.table_value,i);
		if(content)
			avm_assign(lv,content);
		else
			avm_warning("Element with key '",avm_tostring(i),"' could not be on the table",instr->line);
	}
}

void execute_tablesetelem(instr_s * instr){
 
	avm_memcell * t = avm_translate_operand(instr->arg1,(avm_memcell *)NULL);
	avm_memcell * i = avm_translate_operand(instr->arg2,&ax);
	avm_memcell * c = avm_translate_operand(instr->result,&bx);
 
	if(t->type != table_m)
		avm_error("Illegal use of variable (",instr->arg1->name,") as a table",instr->line);

	avm_tablesetelem(&t->data.table_value,i,c);
}
 
void avm_tablesetelem(avm_table ** table,avm_memcell * index,avm_memcell * data){
	avm_table_bucket * temp = NULL;
	unsigned int key;
	if(index->type==string_m){
		key = generate_key(index->data.str_value);
		temp = avm_lookuptable_bystring(*table,index->data.str_value);

		if(temp==NULL){
			temp = malloc(sizeof(avm_table_bucket));
			temp->next = (*table)->str_indexed[key];
			temp->key = malloc(sizeof(avm_memcell));
			temp->value = malloc(sizeof(avm_memcell));
			memcpy(temp->key,index,sizeof(avm_memcell));
		}
		else{
			avm_clear_memcell(temp->value);
		}
		memcpy(temp->value,data,sizeof(avm_memcell));
		if(data->type == string_m){
			temp->value->data.str_value = malloc(strlen(data->data.str_value)+1);
			strcpy(temp->value->data.str_value,data->data.str_value);
		}
		 
		(*table)->str_indexed[key] = temp;
	}
	else{
		if(index->type==integer_m)
			key = index->data.int_value % AVM_TABLE_HASHSIZE;
		else
			key = (int)index->data.double_value % AVM_TABLE_HASHSIZE;
		temp = avm_lookuptable_bynumber(*table,index);
		if(temp==NULL){
			temp = malloc(sizeof(avm_table_bucket));
			temp->next = (*table)->num_indexed[key];
			temp->key = malloc(sizeof(avm_memcell));
			temp->value = malloc(sizeof(avm_memcell));
			memcpy(temp->key,index,sizeof(avm_memcell));
		}
		else{
			avm_clear_memcell(temp->value);
		}
		memcpy(temp->value,data,sizeof(avm_memcell));
		if(data->type == string_m){
			temp->value->data.str_value = malloc(strlen(data->data.str_value));
			strcpy(temp->value->data.str_value,data->data.str_value);
		}
		(*table)->num_indexed[key] = temp;
	}
}
avm_memcell * avm_tablegetitem(avm_table * table,avm_memcell * index){
	avm_table_bucket * temp;
	if(index->type==string_m)
		temp = avm_lookuptable_bystring(table,index->data.str_value);
	else
		temp = avm_lookuptable_bynumber(table,index);

	if(temp)
		return temp->value;
	return NULL;
}

avm_table_bucket * avm_lookuptable_bynumber(avm_table * table,avm_memcell * index){
	unsigned int key;

	if(index->type==integer_m)
		key = index->data.int_value % AVM_TABLE_HASHSIZE;
	else
		key = (int)index->data.double_value % AVM_TABLE_HASHSIZE;

	avm_table_bucket * temp = table->num_indexed[key];

	while(temp!=NULL){
		if(temp->key->type == integer_m && index->type==integer_m && temp){
			if(temp->key->data.int_value == index->data.int_value)
				return temp;
		}
		else if(temp->key->type == integer_m){
			if((double)temp->key->data.int_value == index->data.double_value)
				return temp;
		}
		else if(index->type==integer_m){
			if(temp->key->data.double_value == (double)index->data.int_value)
				return temp;
		}
		else{
			if(temp->key->data.double_value == index->data.double_value)
				return temp;
		}
		temp = temp->next;
	}
	 
	return NULL;
}

avm_table_bucket * avm_lookuptable_bystring(avm_table * table,const char * index){
	unsigned int key = generate_key(index);
	avm_table_bucket * temp = table->str_indexed[key];

	while(temp!=NULL){
		if(strcmp(temp->key->data.str_value,index)==0)
			return temp;
		temp = temp->next;
	}
	return NULL;
}

unsigned int generate_key(const char * name){
	unsigned int sum = 0;
	unsigned int len = strlen(name);
	int i;

	for(i=0;i<len;i++)
		sum += (unsigned int)name[i];

	return sum % AVM_TABLE_HASHSIZE;
}