#include "ProcedureFuncs.h"

#include <string>

#include "ProgramFuncs.h"
#include "SpecialFuncs.h"
#include "Exceptions.h"
#include "TypesFuncs.h"
#include "Funcs.h"

using namespace std;

PreProcedureStepTwo next_step(PreProcedureStepOne p) {
	process_equations(p.program);
	return  PreProcedureStepTwo(process_cmds(p.program, cmd_and_name, cmd_to_args), {},p.args_types(), p.id);
}

void process_procedure_calls(PreProgram& program, const PreProcedureStepOne& called) {
	for (auto& pair : program) {
		auto& str = pair.cmd;
		size_t pos = str.find(called.name());
		if (pos != str.npos) {
			if (count(begin(str), end(str), ',') != (called.args_types().size() ? called.args_types().size() - 1 : called.args_types().size()))
				throw CompileException("not enough arguments in procedure " + called.name());
			str.erase(pos, called.name().size() + 1);
			str.insert(pos, ",");
			str.insert(pos, to_string(called.args_types().size()));
			str.insert(pos, ",");
			str.insert(pos, to_string(called.id));
			str.insert(pos, "(");
			str.insert(pos, "procedure_call");
		}
	}
}

void store_procedure_calls(ListProgram& program, PreProcedureStepTwo& called) {
	for (auto& pair : program) {
		if (pair.cmd == command::procedure_call) {
			if (pair.args[0].val == to_string(called.id())) {
				called.this_procedure_calls.push_back(pair);
			}
		}
	}
}

void process_procedure_vars(ListProgram& program, const vector<cmd_and_args>& vars) {
	if (vars.size()) {
		if (program.empty() || program.front().cmd != command::var) {
			auto line = program.front().line;
			program.insert(begin(program), { command::evar , {},line });
			program.insert(begin(program), { command::var , {},line });
		}

		for (size_t i = 0; i < vars.size(); ++i) {
			program.insert(next(begin(program)), vars[i]);
		}

	}
}

vector<cmd_and_args> process_procedure_args(const args_container& args, size_t line) {
	vector<cmd_and_args> result;
	for (const auto& arg : args) {
		for (const auto& [cmd, code] : type_and_name) {
			if (arg.val.substr(0, cmd.size()) == cmd) {
				auto sub_arg_str = arg.val.substr(cmd.size(), arg.val.size() - cmd.size());
				result.push_back({ code, str_to_args(sub_arg_str), line });
				break;
			}
		}
	}
	return result;
}

deque<arg_info> gen_procedure_call_args(const vector<cmd_and_args>& args) {
	if (args.empty())return {};

	deque<arg_info> result;
	for (const auto& arg : args) {
		result.insert(end(result), begin(arg.args), end(arg.args));
	}
	return result;
}

vector<cmd_and_args> process_vars_and_call_args(PreProcedureStepTwo& p) {
	vector<cmd_and_args> args;
	if (p.args_types().size()) {
		args = process_procedure_args(p.args_types(), p.program.front().line - 2);
		auto args_arr = gen_procedure_call_args(args);
		process_procedure_vars(p.program, args);
		for (size_t i = 0; i < p.this_procedure_calls.size(); ++i) {
			p.program.push_back({ command::nop, args_arr, 0 });
		}
		process_variables(p.program);
		for (int i = p.this_procedure_calls.size() - 1; i >= 0; --i) {
			auto& lbuf = p.this_procedure_calls[i].get().args;
			auto iter = prev(p.program.end());
			advance(iter, -i);
			auto& rbuf = (iter)->args;
			lbuf.insert(end(lbuf), begin(rbuf), end(rbuf));
		}
		p.program.erase(remove_if(begin(p.program), end(p.program), [](const cmd_and_args& rh) { return rh.cmd == command::nop; }), end(p.program));
	}
	else {
		process_variables(p.program);
	}
	return args;
}

Procedure compile_procedure(PreProcedureStepTwo p, const vector<cmd_and_args>& args) {
	p.program.push_back({ command::end_procedure, deque<arg_info>{ {"",true, false}}, 0 });
	process_args(p.program, cmd_codes, arg_process);
	process_sizes(p.program);
	process_ifs_else(p.program);
	return { p.program, args , p.id() };
}

ListProgram gen_procedure_table(const map<size_t,Procedure>& procedures) {
	ListProgram result{ {command::procedure, {}, 0} };
	result.back().args.push_back({ num_to_str(procedures.size()), true, true });

	uint16_t offset = 0u;
	size_t id = 0;
	//static auto find_by_id = [&procedures](size_t id) {
	//	return find_if(begin(procedures), end(procedures), [id](const pair<size_t,Procedure>& rh) { return rh.id() == id; });
	//};

	for (auto& [id, procedure] : procedures) {
		result.back().args.push_back({ num_to_str(procedure.args_types().size()), true, true });
		result.back().args.push_back({ num_to_str(offset) , true, true });

		offset += procedure.get_size();
	}
	process_sizes(result);
	return result;
}