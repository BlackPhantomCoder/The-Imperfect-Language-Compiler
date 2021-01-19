#include "Procedure/Procedure.h"

PreProcedureStepOne::PreProcedureStepOne(
	const PreProgram& program,
	const std::string& name,
	const args_container& args_types,
	size_t id
):
	program(program),
	t_name(name),
	t_args_types(args_types),
	id(id)
	{}

PreProcedureStepTwo::PreProcedureStepTwo(
	const ListProgram& program,
	const std::vector<std::reference_wrapper<cmd_and_args>>& calls,
	const args_container& args_types,
	size_t id
):
	program(program),
	this_procedure_calls(calls),
	t_args_types(args_types),
	t_id(id)
	{}

bool PreProcedureStepTwo::operator<(const PreProcedureStepTwo & rh)
{
	return t_id < rh.t_id;
}

Procedure::Procedure(
	const ListProgram & program,
	const std::vector<cmd_and_args> & args_types,
	size_t id
): 
	program(program),
	t_args_types(args_types),
	t_id(id)
	{}

size_t Procedure::get_size() const
{
	size_t result = 0;
	for (const auto& [cmd, args, line] : program) {
		result += 1;
		result += args.args_size();
	}
	return result;
}

bool Procedure::operator<(const Procedure& rh)
{
	return t_id < rh.t_id;
}
