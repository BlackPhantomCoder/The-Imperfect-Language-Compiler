#pragma once
#include <cstdint>
#include <string>
#include <iostream>
#include <set>
#include <map>
#include <functional>
#include <fstream>
#include <vector>
#include <utility>
#include <list>

#include "Program.h"
#include "Procedure.h"


//input program
PreProgram parse_program(std::istream& in);
//cleaning program from some chars
void clear_program(PreProgram& program, const std::set<char>& chars);
//parsing Procedures (step one) 
std::vector<PreProcedureStepOne> get_PreProcedures(const PreProgram& program);
//parsing cmds
ListProgram process_cmds(
	const PreProgram& program,
	const std::map<std::string, command>& cmd_codes,
	const std::map<uint8_t, int>& cmd_to_args
);
//process cmds arguments
void process_args(
	ListProgram& program,
	const std::set<std::uint8_t>& cmds,
	const std::function<args_container(std::uint8_t, args_container)>& fnc
);
//storing sizes of args
void process_sizes(ListProgram& program);
//program to ostream
void output_program(const ListProgram& program, std::ostream& out);