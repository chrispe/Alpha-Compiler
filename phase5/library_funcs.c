#include "library_funcs.h"

void libfunc_print(void){
	unsigned int n = avm_total_actuals();
	unsigned int i;
	for(i=0;i<n;i++){
		char * s = avm_tostring(avm_get_actual(i));
		printf("%s",s);
		 
	}
}