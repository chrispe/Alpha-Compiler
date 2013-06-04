#include "library_funcs.h"

void libfunc_print(){
	unsigned int n = avm_total_actuals();
	unsigned int i;
	for(i=0;i<n;i++){
		char * s = avm_tostring(avm_get_actual(i));
		printf("%s",s);
	}
}

void libfunc_totalarguments(){
	unsigned p_topsp = avm_get_env_value(topsp+AVM_SAVEDTOPSP_OFFSET);
	avm_clear_memcell(&retval);

	if(!p_topsp){
		avm_error("Library function 'totalarguments' was called outside of a function","","",instructions[pc].line);
		retval.type = nil_m;
	}
	else{
		retval.type = integer_m;
		retval.data.int_value = avm_get_env_value(p_topsp+AVM_NUMACTUALS_OFFSET);
	}

}