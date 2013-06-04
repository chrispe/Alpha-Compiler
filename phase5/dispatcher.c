#include "dispatcher.h"

unsigned char execution_finished = 0;
unsigned int pc = 0;
unsigned int curr_line = 0;

void execute_cycle(void){
	if(execution_finished)
		return;
	else{
		if(pc == AVM_ENDING_PC){
			execution_finished = 1;
			return;
		}
		else{
			assert(pc<AVM_ENDING_PC);
			instr_s * instr = instructions + pc;
			curr_line = instr->line;
			unsigned int old_pc = pc;
			execute_instr(instr);
			if(pc==old_pc)
				pc++;
		}
	}
}

void execute_instr(instr_s * instr){
	switch(instr->opcode){
		case assign_v: execute_assign(instr); break;
		case call_v: execute_call(instr); break;
		case funcenter_v: execute_funcenter(instr); break;
		case funcexit_v: execute_funcexit(instr); break;
		case pusharg_v: execute_pusharg(instr); break;
		case add_v: execute_arithmetic(instr); break;
		case sub_v: execute_arithmetic(instr); break;
		case mul_v: execute_arithmetic(instr); break;
		case div_v: execute_arithmetic(instr); break;
		case mod_v: execute_arithmetic(instr); break;
		case jump_v: execute_jump(instr); break;
		case jeq_v: execute_jeq(instr); break;
		case jne_v:execute_jeq(instr); break;
		case jgt_v:execute_cmp(instr); break;
		case jge_v:execute_cmp(instr); break;
		case jlt_v:execute_cmp(instr); break;
		case jle_v:execute_cmp(instr); break;
		case newtable_v: execute_newtable(instr); break;
		case tablegetelem_v: execute_tablegetelem(instr); break;
		case tablesetelem_v: execute_tablesetelem(instr); break;
		default: return;
	}
}