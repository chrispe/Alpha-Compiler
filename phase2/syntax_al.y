%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "symbol_table.h"
%}

%%


%%

int main(int argc,char ** argv)
{
	symbol_table * sb;
	sb = create_symbol_table();
	return 0;	
}