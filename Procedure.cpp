#include "Procedure.h"

//std::ostream& operator<< (std::ostream& os, const Procedure& procedure) {
//	for (const auto& [cmd, args, line ] : procedure.program) {
//		os << cmd << args;
//	}
//	return os;
//}

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
		result += 1;// sizeof(cmd);
		result += args.args_size();
		/*if (args.empty())continue;
		uint8_t arg_size = args[0].val.size();
		if (args.size() > 1) {
			for (size_t i = 1; i < args.size(); ++i) {
				arg_size += args[i].val.size() + 1;
			}
			arg_size -= 1;
		}
		result += arg_size;*/
	}
	return result;
}
