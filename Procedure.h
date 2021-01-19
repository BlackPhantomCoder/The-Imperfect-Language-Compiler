#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include <iostream>

#include "Program.h"

//PreProcedure (first step) class
class PreProcedureStepOne
{
public:
	PreProcedureStepOne(
		const PreProgram& program,
		const std::string& name,
		const args_container& args_types, 
		size_t id
	);

	PreProgram program;
	const std::string& name()const { return t_name; }
	const args_container& args_types()const { return t_args_types; }
	//
	size_t id;
private:
	std::string t_name;
	args_container t_args_types;
};

//PreProcedure (second step) class
class PreProcedureStepTwo
{
public:
	PreProcedureStepTwo(
		const ListProgram& program,
		const std::vector <std::reference_wrapper<cmd_and_args>>& calls,
		const args_container& args_types,
		size_t id
	);

	ListProgram program;
	std::vector<std::reference_wrapper<cmd_and_args>> this_procedure_calls;
	const args_container& args_types()const { return t_args_types; }
	//
	const size_t id()const { return t_id; }
	bool operator<(const PreProcedureStepTwo& rh);
private:
	args_container t_args_types;
	size_t t_id;
};

// Procedure class
class Procedure
{
public:
	Procedure(
		const ListProgram& program,
		const std::vector<cmd_and_args>& args_types,
		size_t id
	);
	ListProgram program;
	const std::vector<cmd_and_args>& args_types()const { return t_args_types; }
	size_t get_size() const;
	//
	const size_t id()const { return t_id; }
	bool operator<(const Procedure& rh);
private:
	std::vector<cmd_and_args> t_args_types;
	size_t t_id;
};