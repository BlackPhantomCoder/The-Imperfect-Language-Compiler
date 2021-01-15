#pragma once
#include <vector>
#include <deque>

#include "Procedure.h"
#include "Program.h"

//PreProcedureStepOne to PreProcedureStepTwo (with processing equations)
PreProcedureStepTwo next_step(PreProcedureStepOne p);
//replasing name of procedure to cmd call
void process_procedure_calls(PreProgram& program, const PreProcedureStepOne& called);
//storing calls
void store_procedure_calls(ListProgram& program, PreProcedureStepTwo& called);
//including vars from args to vars block
void process_procedure_vars(ListProgram& program, const std::vector<cmd_and_args>& vars);
//procedure args to array of cmds (type) and args(name)
std::vector<cmd_and_args> process_procedure_args(const args_container& args, size_t line);
//generating procedure call arguments
std::deque<arg_info> gen_procedure_call_args(const std::vector<cmd_and_args>& args);
//processing call vars/ vars
std::vector<cmd_and_args> process_vars_and_call_args(PreProcedureStepTwo& p);
//PreProcedureStepTwo to Procedure with (end_procedure, processing arguments, processing argument sizes, processing if/else contruction)
Procedure compile_procedure(PreProcedureStepTwo p, std::vector<cmd_and_args> args);
//generating procedure table block
ListProgram gen_procedure_table(std::vector<Procedure>& procedures);