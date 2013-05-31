#include "file_handler.h"

// For the number constants 
double * double_consts = NULL;
unsigned int current_double_index = 0;
unsigned int total_double_consts = 0;

// For the integer number constants
int * integer_consts = NULL;
unsigned int current_int_index = 0;
unsigned int total_integer_consts = 0;

// For the strings
char ** str_consts = NULL;
unsigned int current_str_index = 0;
unsigned int total_str_consts = 0;

// For the library functions
char ** named_lib_funcs = NULL;
unsigned int current_lib_func_index = 0;
unsigned int total_named_lib_funcs = 0;

// For the user functions
userfunc_s * user_funcs = NULL;
unsigned int current_user_func_index = 0;
unsigned int total_user_funcs = 0;

/* The array that will include 
   the final target instructions. */
instr_s * instructions = NULL;
unsigned int current_instr_index = 0;
unsigned int total_instructions = 0;

void read_binary_file(char * filename){
	FILE * source;
	unsigned int i;

    if ((source = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "\nError : Cannot read binary file %s\n", filename);
        exit(0);
    }

 	read_magic_number(source);
 	read_arrays(source);
 	read_instructions(source);
 	fclose(source);
 	print_instructions();
}

void read_arrays(FILE * source){
	read_strings(source);
	read_integers(source);
	read_doubles(source);
	read_user_functions(source);
	read_lib_functions(source);
}

void read_strings(FILE * source){
	unsigned int i;
	unsigned int length;
	fread(&total_str_consts,sizeof(unsigned int),1,source);
	if(total_str_consts>0){
		str_consts = (char **)malloc(total_str_consts*sizeof(char *));
		for(i=0;i<total_str_consts;i++){
			fread(&length,sizeof(unsigned int),1,source);
			str_consts[i] = malloc(length+1);
			fread(str_consts[i],length+1,1,source);
		}
	}
}

void read_integers(FILE * source){
	unsigned int i;
	fread(&total_integer_consts,sizeof(unsigned int),1,source);
	if(total_integer_consts>0){
		integer_consts = (int *)malloc(total_integer_consts*sizeof(int));
		for(i=0;i<total_integer_consts;i++){
			fread(&integer_consts[i],sizeof(int),1,source);
		}
	}
}

void read_doubles(FILE * source){
	unsigned int i;
	fread(&total_double_consts,sizeof(unsigned int),1,source);
	if(total_double_consts>0){
		double_consts = (double *)malloc(total_double_consts*sizeof(double));
		for(i=0;i<total_double_consts;i++){
			fread(&double_consts[i],sizeof(double),1,source);
		}
	}	
}

void read_user_functions(FILE * source){
	unsigned int i;
	unsigned int length;
	fread(&total_user_funcs,sizeof(unsigned int),1,source);
	if(total_user_funcs>0){
		user_funcs = (userfunc_s *)malloc(total_user_funcs*sizeof(userfunc_s));
		for(i=0;i<total_user_funcs;i++){
			fread(&(user_funcs[i].address),sizeof(unsigned int),1,source);
			fread(&(user_funcs[i].local_size),sizeof(unsigned int),1,source);
			fread(&length,sizeof(unsigned int),1,source);
			user_funcs[i].name = malloc(length+1);
			fread((user_funcs[i].name),length+1,1,source);
		}
	}	
}

void read_lib_functions(FILE * source){
	unsigned int i;
	unsigned int length;
	fread(&total_named_lib_funcs,sizeof(unsigned int),1,source);
	if(total_named_lib_funcs>0){
		named_lib_funcs = (char **)malloc(total_named_lib_funcs*sizeof(char *));
		for(i=0;i<total_named_lib_funcs;i++){
			fread(&length,sizeof(unsigned int),1,source);
			named_lib_funcs[i] = malloc(length+1);
			fread(named_lib_funcs[i],length+1,1,source);
		}
	}
}

void read_magic_number(FILE * source){
	unsigned int magic_number;
	fread(&magic_number,sizeof(unsigned int),1,source);
	if(magic_number!=MAGIC_NUMBER)
		error_message("Wrong magic number.");
}

void read_instructions(FILE * source){
	unsigned int i;
	unsigned int total_args;
	fread(&total_instructions,sizeof(unsigned int),1,source);
	if(total_instructions>0){
		instructions = (instr_s *)malloc(sizeof(instr_s)*total_instructions);
		for(i=0;i<total_instructions;i++){
			fread(&instructions[i].opcode,1,1,source);
			total_args = get_instr_arg_num(instructions[i].opcode);
			switch(total_args){
				case 3:{
					instructions[i].arg1 = create_vmarg();
					fread(&instructions[i].arg1->type,1,1,source);
					fread(&instructions[i].arg1->value,sizeof(unsigned int),1,source);
					instructions[i].arg2 = create_vmarg();
					fread(&instructions[i].arg2->type,1,1,source);
					fread(&instructions[i].arg2->value,sizeof(unsigned int),1,source);
					instructions[i].result = create_vmarg();
					fread(&instructions[i].result->type,1,1,source);
					fread(&instructions[i].result->value,sizeof(unsigned int),1,source);
					break;
				}
				case 2:{
					instructions[i].arg1 = create_vmarg();
					fread(&instructions[i].arg1->type,1,1,source);
					fread(&instructions[i].arg1->value,sizeof(unsigned int),1,source);
					instructions[i].arg2 = NULL;
					instructions[i].result = create_vmarg();
					fread(&instructions[i].result->type,1,1,source);
					fread(&instructions[i].result->value,sizeof(unsigned int),1,source);
					break;
				}
				case 1:{
					instructions[i].arg1 = NULL;
					instructions[i].arg2 = NULL;
					instructions[i].result = create_vmarg();
					fread(&instructions[i].result->type,1,1,source);
					fread(&instructions[i].result->value,sizeof(unsigned int),1,source);
					break;
				}
				case 0:{
					instructions[i].arg1 = NULL;
					instructions[i].arg2 = NULL;
					instructions[i].result = NULL;	
					break;
				}
				default: assert(0);
			}
		}
	}
}

void error_message(char * msg){
	fprintf(stderr, "\nError : %s\n", msg);
	exit(0);
}

unsigned int add_const_to_array(void * constant,const_t type){
	switch(type){
		case double_c:{
			if(current_double_index == total_double_consts)
				expand_const_array(type);
			double * current_double = double_consts + current_double_index++;
			*current_double = *((double *)(constant));
			return(current_double_index-1);
		}
		case int_c:{
			if(current_int_index == total_integer_consts)
				expand_const_array(type);
			int * current_int = integer_consts + current_int_index++;
			*current_int = *((int *)(constant));
			return(current_int_index-1);
		}
		case str_c:{
			if(current_str_index == total_str_consts)
			expand_const_array(type);
			char *  current_str = malloc(strlen((char*)constant)+1);
			strcpy(current_str,(char*)constant);
			str_consts[current_str_index++] = current_str;
			return(current_str_index-1);
		}
		case user_func_c:{
			if(current_user_func_index == total_user_funcs)
				expand_const_array(type);
			userfunc_s * current_user_func = user_funcs + current_user_func_index++;
			current_user_func->address = ((userfunc_s *)constant)->address;
			current_user_func->local_size = ((userfunc_s *)constant)->local_size;
			current_user_func->name = malloc(strlen(((userfunc_s *)constant)->name)+1);
			strcpy(current_user_func->name,((userfunc_s *)constant)->name);
			return(current_user_func_index-1);
		}
		case lib_func_c:{
			if(current_lib_func_index == total_named_lib_funcs)
				expand_const_array(type);
			char * current_lib_func = malloc(strlen((char *)constant)+1);
			strcpy(current_lib_func,(char *)constant);
			named_lib_funcs[current_lib_func_index++] = current_lib_func;
			return(current_lib_func_index-1);
		}
		default: assert(0);
	}
}

void expand_const_array(const_t array_type){
	switch(array_type){
		case double_c:{
			double * new_double_arr = (double *)malloc(DOUBLE_ARR_NEW_SIZE);
			if(memerror(new_double_arr,"new double array"))exit(0);
			memcpy(new_double_arr,double_consts,DOUBLE_ARR_SIZE);
			double_consts = new_double_arr;
			total_double_consts += EXPAND_SIZE;
			break;
		}
		case int_c:{
			int * new_int_arr = (int *)malloc(INTGER_ARR_NEW_SIZE);
			if(memerror(new_int_arr,"new integer array"))exit(0);
			memcpy(new_int_arr,integer_consts,INTEGER_ARR_SIZE);
			integer_consts = new_int_arr;
			total_integer_consts += EXPAND_SIZE;
			break;
		}
		case str_c:{
			char ** new_str_arr = (char **)malloc(STRING_ARR_NEW_SIZE);
			if(memerror(new_str_arr,"new string array"))exit(0);
			memcpy(new_str_arr,str_consts,STRING_ARR_SIZE);
			str_consts = new_str_arr;
			total_str_consts += EXPAND_SIZE;
			break;
		}
		case user_func_c:{
			userfunc_s * new_user_func_arr = (userfunc_s *)malloc(USER_FUNC_ARR_NEW_SIZE);
			if(memerror(new_user_func_arr,"new user func array"))exit(0);
			memcpy(new_user_func_arr,user_funcs,USER_FUNC_ARR_SIZE);
			user_funcs = new_user_func_arr;
			total_user_funcs += EXPAND_SIZE;
			break;
		}
		case lib_func_c:{
			char ** new_lib_func_arr = (char **)malloc(LIB_FUNC_ARR_NEW_SIZE);
			if(memerror(new_lib_func_arr,"new lib func array"))exit(0);
			memcpy(new_lib_func_arr,named_lib_funcs,LIB_FUNC_ARR_SIZE);
			named_lib_funcs = new_lib_func_arr;
			total_named_lib_funcs += EXPAND_SIZE;
			break;
		}
		default: assert(0);
	}
}

int memerror(void * ptr, const char * name){
	if(ptr==NULL){
		printf("Error : Memory allocation for the <%s> failed.\n",name);
		return 1;
	}
	return 0;
}

unsigned int get_instr_arg_num(vmopcode_e op){
	if(op==add_v||op==sub_v||op==mul_v||op==div_v||op==mod_v||op==jeq_v||op==jne_v||op==jle_v
	||op==jge_v||op==jlt_v||op==jgt_v||op==tablesetelem_v||op==tablegetelem_v)
		return 3;
	else if(op==assign_v)
		return 2;
	else if(op==call_v||op==pusharg_v||op==funcenter_v||op==funcexit_v||op==jump_v||op==newtable_v)
		return 1;
	else if(op==nop_v)
		return 0;
	else 
		assert(0);
}

vmarg_s * create_vmarg(void){
	vmarg_s * new_vmarg = malloc(sizeof(vmarg_s));
	if(memerror(new_vmarg,"new vmarg"))exit(0);
	return(new_vmarg);
}
 
instr_s * create_instr(void){
	instr_s * new_instr = malloc(sizeof(instr_s));
	if(memerror(new_instr,"new instr"))exit(0);
	new_instr->arg1 = NULL;
	new_instr->arg2 = NULL;
	new_instr->result = NULL;
	return(new_instr);
}

char * vm_opcode_to_str(vmopcode_e op){
	char * ops[] = {"ASSIGN","ADD","SUB","MUL","DIV","MOD","UMINUS","AND","OR","NOT",
					"JEQ","JNE","JLE","JGE","JLT","JGT",
					"CALLFUNC","PUSHARG","RET","GETRETVAL","ENTERFUNC","EXITFUNC",
					"JUMP","NEWTABLE","TABLEGETELEM","TABLESETITEM","NOP" };
	return(ops[op-assign_v]);
}

void print_instructions()
{
	int i;
	FILE * quads_output; 
 
	quads_output = fopen("target_code.txt","w"); 
	if(quads_output==NULL)
		quads_output = stderr;  

	fprintf(quads_output,"<user functions>: \n");
	for(i=0;i<total_user_funcs;i++){
		fprintf(quads_output,"%d : %s\n",i,user_funcs[i].name);
	}
	fprintf(quads_output,"<end of funcs>\n");	

	fprintf(quads_output,"<lib functions>: \n");
	for(i=0;i<total_named_lib_funcs;i++){
		fprintf(quads_output,"%d : %s\n",i,named_lib_funcs[i]);
	}
	fprintf(quads_output,"<end of lib funcs>\n");	

	fprintf(quads_output,"<strings> : \n");
	for(i=0;i<total_str_consts;i++){
		fprintf(quads_output,"%d : %s\n",i,str_consts[i]);
	}
	fprintf(quads_output,"<end of strings>\n");

	fprintf(quads_output,"<integers> : \n");
	for(i=0;i<total_integer_consts;i++){
		fprintf(quads_output,"%d : %d\n",i,integer_consts[i]);
	}
	fprintf(quads_output,"<end of integers>\n");
	fprintf(quads_output,"<doubles> : \n");
	for(i=0;i<total_double_consts;i++){
		fprintf(quads_output,"%d : %lf\n",i,double_consts[i]);
	}
	fprintf(quads_output,"<end of doubles>\n");

	fprintf(quads_output,"number of strings %d\n",current_str_index);
	for(i=0;i<total_instructions;i++){
			fprintf(quads_output,"%d:\t%s",i,vm_opcode_to_str(instructions[i].opcode));
			if(instructions[i].arg1){
				fprintf(quads_output," %d (%s) ",instructions[i].arg1->value,value_type_to_str(instructions[i].arg1->type));
			}
			if(instructions[i].arg2){
				fprintf(quads_output," %d (%s) ",instructions[i].arg2->value,value_type_to_str(instructions[i].arg2->type));
			}
			if(instructions[i].result){
				fprintf(quads_output," %d (%s) ",instructions[i].result->value,value_type_to_str(instructions[i].result->type));
			}
			fprintf(quads_output,"\n");
	}
	fclose(quads_output);
}

char * value_type_to_str(vmarg_t type){
	char * value_types[] = {"label_a","global_a","formal_a","local_a",
							"integer_a" ,"double_a" ,"string_a" ,"bool_a",
							"nil_a" , "userfunc_a" ,"libfunc_a" ,"retval_a"};
	return(value_types[type]);
} 