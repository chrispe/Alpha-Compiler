#ifndef ENV_MEM_LIB
#include "env_memory.h"
#endif
 
#define DISPATCHER

/* A boolean if the execution has finished */
extern unsigned char execution_finished;

/* The program counter of the AVM */
extern unsigned int pc;

/* The current line of the instructions */
extern unsigned int curr_line;

#define AVM_ENDING_PC total_instructions

/* Executes a cycle of the AVM */
void execute_cycle(void);

/* Executes the instruction argument */
void execute_instr(instr_s *);