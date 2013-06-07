#include "library_funcs.h"

void libfunc_print(){
	unsigned int n = avm_total_actuals();
	unsigned int i;
	for(i=0;i<n;i++){
		char * s = avm_tostring(avm_get_actual(i));
		if(avm_get_actual(i)->type==userfunc_m)
			printf("User function : ");
		else if(avm_get_actual(i)->type==libfunc_m)
			printf("Library function : ");
		printf("%s",s);
	}
}

void libfunc_totalarguments(){
	unsigned p_topsp = avm_get_env_value(topsp+AVM_SAVEDTOPSP_OFFSET);
	avm_clear_memcell(&retval);

	if(!p_topsp){
		avm_warning("Library function 'totalarguments' was called outside of a function, will return nil.","","",instructions[pc].line);
		retval.type = nil_m;
		return;
	}

	retval.type = integer_m;
	retval.data.int_value = avm_get_env_value(p_topsp+AVM_NUMACTUALS_OFFSET);
}

void libfunc_argument(){
	unsigned int n = avm_total_actuals();
	unsigned int p_topsp = avm_get_env_value(topsp+AVM_SAVEDTOPSP_OFFSET);

	if(!p_topsp){
		avm_warning("Library function 'argument(i)' was called outside of a function, will return nil","","",instructions[pc].line);
		retval.type = nil_m;
		return;
	}

	if(n==0)
		avm_error("Library function 'argument(i)' was called without an argument","","",instructions[pc].line);

	avm_memcell * arg = avm_get_actual(0);
	char arg_valid = (arg->type == integer_m || arg->type== double_m);
	if(!arg_valid)
		avm_error("Library function 'argument(i)' was called with invalid type of argument, only numbers are allowed","","",instructions[pc].line);
	avm_assign(&retval,&stack[p_topsp+AVM_NUMACTUALS_OFFSET+1+arg->data.int_value],instructions[pc].line);
}

void libfunc_typeof(){
	unsigned int n = avm_total_actuals();
	if(n==0){
		avm_warning("Library function 'typeof(var)' was called without an argument, will return nil","","",instructions[pc].line);
		retval.type = nil_m;
	}

	avm_memcell * arg = avm_get_actual(0);
	retval.type = string_m;
	retval.data.str_value = real_value_type_to_str(arg->type);
}

void libfunc_input(){
	unsigned int i;
	unsigned int len;
	char input_str[512];
	scanf("%s",input_str);
	len = strlen(input_str);

	unsigned int dots = 0;
	char recognized_char = 0;

	for(i=0;i<len;i++){
		if(input_str[i]=='.')
			dots++;
		else if((int)input_str[i]>(int)('9') || (int)input_str[i]<(int)('0'))
			recognized_char = 1;
	}

	if(strcmp(input_str,"true")==0){
		retval.type = bool_m;
		retval.data.bool_value = 1;
	}
	else if(strcmp(input_str,"false")==0){
		retval.type = bool_m;
		retval.data.bool_value = 0;
	}
	else if(strcmp(input_str,"nil")==0){
		retval.type = nil_m;
	}
	else if(recognized_char || dots>1){
		retval.type = string_m;
		retval.data.str_value = malloc(len+1);
		strcpy(retval.data.str_value,input_str);
	}
	else if(dots==0){
		retval.type = integer_m;
		retval.data.int_value = atoi(input_str);
	}
	else{
		retval.type = double_m;
		retval.data.double_value = atof(input_str);
	}
}

void libfunc_sin(){
	unsigned int n = avm_total_actuals();
	if(n==0)
		avm_error("Library function 'sin(x)' was called without an argument","","",instructions[pc].line);

	avm_memcell * arg = avm_get_actual(0);

	char arg_valid = (arg->type == integer_m || arg->type== double_m);
	if(!arg_valid)
		avm_error("Library function 'sin(x)' was called with invalid type of argument, only numbers are allowed","","",instructions[pc].line);

	double arg_value;
	if(arg->type==integer_m)
		arg_value = (double)arg->data.int_value;
	else
		arg_value = arg->data.double_value;

	retval.type = double_m;
	double radians = arg_value * M_PI/180.0f;
	retval.data.double_value = sin(radians);	 	
}

void libfunc_cos(){
	unsigned int n = avm_total_actuals();
	if(n==0)
		avm_error("Library function 'cos(x)' was called without an argument","","",instructions[pc].line);

	avm_memcell * arg = avm_get_actual(0);

	char arg_valid = (arg->type == integer_m || arg->type== double_m);
	if(!arg_valid)
		avm_error("Library function 'cos(x)' was called with invalid type of argument, only numbers are allowed","","",instructions[pc].line);

	double arg_value;
	if(arg->type==integer_m)
		arg_value = (double)arg->data.int_value;
	else
		arg_value = arg->data.double_value;

	retval.type = double_m;
	double radians = arg_value * M_PI/180.0f;
	retval.data.double_value = cos(radians);	 	
}

void libfunc_sqrt(){
	unsigned int n = avm_total_actuals();
	if(n==0)
		avm_error("Library function 'sqrt(x)' was called without an argument","","",instructions[pc].line);

	avm_memcell * arg = avm_get_actual(0);
	
	char arg_valid = (arg->type == integer_m || arg->type== double_m);
	if(!arg_valid)
		avm_error("Library function 'sqrt(x)' was called with invalid type of argument, only numbers are allowed","","",instructions[pc].line);

	double arg_value;
	if(arg->type==integer_m)
		arg_value = (double)arg->data.int_value;
	else
		arg_value = arg->data.double_value;

	if(arg_value<0){
		retval.type = nil_m;
		return;
	}

	retval.type = double_m;
	retval.data.double_value = sqrt(arg_value);	 
}

void libfunc_strtonum(){
	unsigned int n = avm_total_actuals();
	if(n==0)
		avm_error("Library function 'strtonum(x)' was called without an argument","","",instructions[pc].line);
	
	avm_memcell * arg = avm_get_actual(0);
	if(arg->type!=string_m)
		avm_error("Library function 'strtonum(s)' was called with invalid type of argument, only strings are allowed","","",instructions[pc].line);
	
	unsigned int len = strlen(arg->data.str_value);

	unsigned int i;
	unsigned int dots = 0;
	char recognized_char = 0;

	for(i=0;i<len;i++){
		if(arg->data.str_value[i]=='.')
			dots++;
		else if((int)arg->data.str_value[i]>(int)('9') || (int)arg->data.str_value[i]<(int)('0'))
			recognized_char = 1;
	}

	if(recognized_char || dots>1){
		avm_warning("The string",arg->data.str_value,"does not represent a number, so the strtonum(s) will return nil.",instructions[pc].line);
		retval.type = nil_m;
	}
	else if(dots==0){
		retval.type = integer_m;
		retval.data.int_value = atoi(arg->data.str_value);
	}
	else{
		retval.type = double_m;
		retval.data.double_value = atof(arg->data.str_value);
	}
}

void libfunc_objecttotalmembers(){
	unsigned int n = avm_total_actuals();
	if(n==0)
		avm_error("Library function 'objecttotalmembers(object)' was called without an argument","","",instructions[pc].line);

	avm_memcell * arg = avm_get_actual(0);
	if(arg->type!=table_m)
		avm_error("Library function 'objecttotalmembers(object)' was called with invalid type of argument, only tables are allowed","","",instructions[pc].line);

	unsigned int i;
	unsigned int total_objects = 0;
	avm_table_bucket * temp;  

	for(i=0;i<AVM_TABLE_HASHSIZE;i++){
		temp = arg->data.table_value->num_indexed[i];
		while(temp){
			total_objects++;
			temp = temp->next;
		}
	}

	for(i=0;i<AVM_TABLE_HASHSIZE;i++){
		temp = arg->data.table_value->str_indexed[i];
		while(temp){
			total_objects++;
			temp = temp->next;
		}
	}

	retval.type = integer_m;
	retval.data.int_value = total_objects;
}

void libfunc_objectmemberkeys(){
	unsigned int n = avm_total_actuals();
	if(n==0)
		avm_error("Library function 'objectmemberkeys(object)' was called without an argument","","",instructions[pc].line);

	avm_memcell * arg = avm_get_actual(0);
	if(arg->type!=table_m)
		avm_error("Library function 'objectmemberkeys(object)' was called with invalid type of argument, only tables are allowed","","",instructions[pc].line);

	retval.type = table_m;

	retval.data.table_value = avm_newtable();

	unsigned int i;
	unsigned int total_objects = 0;
	avm_table_bucket * temp;  

	avm_memcell index;
	index.type = integer_m;

	for(i=0;i<AVM_TABLE_HASHSIZE;i++){
		temp = arg->data.table_value->num_indexed[i];
		while(temp){
			index.data.int_value = total_objects;
			avm_tablesetelem(&retval.data.table_value,&index,temp->key); 
			total_objects++;
			temp = temp->next;
		}
	}

	for(i=0;i<AVM_TABLE_HASHSIZE;i++){
		temp = arg->data.table_value->str_indexed[i];
		while(temp){
			index.data.int_value = total_objects;
			avm_tablesetelem(&retval.data.table_value,&index,temp->key); 
			total_objects++;
			temp = temp->next;
		}
	}
}

void libfunc_objectcopy(){
	unsigned int n = avm_total_actuals();
	if(n==0)
		avm_error("Library function 'objectcopy(object)' was called without an argument","","",instructions[pc].line);

	avm_memcell * arg = avm_get_actual(0);
	if(arg->type!=table_m)
		avm_error("Library function 'objectcopy(object)' was called with invalid type of argument, only tables are allowed","","",instructions[pc].line);

	retval.type = table_m;

	retval.data.table_value = avm_newtable();

	unsigned int i;
	avm_table_bucket * temp;  
 
	for(i=0;i<AVM_TABLE_HASHSIZE;i++){
		temp = arg->data.table_value->num_indexed[i];
		while(temp){
			avm_tablesetelem(&retval.data.table_value,temp->key,temp->value); 
			temp = temp->next;
		}
	}

	for(i=0;i<AVM_TABLE_HASHSIZE;i++){
		temp = arg->data.table_value->str_indexed[i];
		while(temp){
			avm_tablesetelem(&retval.data.table_value,temp->key,temp->value); 
			temp = temp->next;
		}
	}
}