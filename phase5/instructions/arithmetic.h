#ifndef ENV_MEM_LIB
#include "../env_memory.h"
#endif

#ifndef DISPATCHER
#include "../dispatcher.h"
#endif

typedef double (*arithmetic_func_t)(double x,double y);

extern arithmetic_func_t arithmetic_funcs[];

double add_impl (double,double);
double sub_impl (double, double);
double mul_impl (double,double);
double div_impl (double,double);
double mod_impl(double,double);

void execute_arithmetic(instr_s *);