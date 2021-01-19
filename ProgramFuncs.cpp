#include "ProgramFuncs.h"

#include <algorithm>
#include "Exceptions.h"
#include "Funcs.h"

using namespace std;

void clean_str(string& str, const set<char>& chars) {
	for (const auto& ch : chars) {
		string::iterator iter;
		while ((iter = find(begin(str), end(str), ch)) != end(str)) {
			str.erase(iter);
		}
	}
}

PreProgram parse_program(std::istream& in)
{
	PreProgram result;
	string buf;
	size_t i = 1;
	while (getline(in, buf)) {
		result.push_back({ buf,  i++});
	}
	return result;
}

void clear_program(PreProgram& program, const set<char>& chars)
{
	size_t i = 0;
	while (true) {
		if (i == program.size())break;
		clean_str(program[i].cmd, chars);
		if (program[i].cmd.empty()) {
			program.erase(begin(program) + i);
		}
		else {
			++i;
		}
	}
}

std::vector<PreProcedureStepOne> get_PreProcedures(const PreProgram& program)
{
	static const string cmd = "procedure";
	vector<PreProcedureStepOne> result;
	for (auto i = begin(program); i != end(program); ++i) {
		string str = i->cmd;
		if (cmd.size() <= str.size() && cmd == str.substr(0, cmd.size())) {
			str.erase(0, cmd.size());

			const auto name = str.substr(0, str.find('('));
			str.erase(0, str.find('(') + 1);

			auto args_str = str.substr(0, str.find(')'));
			str.erase(0, str.find(')') + 1);

			result.push_back({ {}, name, str_to_args(args_str),result.size() });
			
			++i;
			if (i == end(program) || i->cmd != "{")
				throw ProgramException(string("Procedure ") +  result.back().name() + " is not full ( not enough { )", i->line);
			++i;

			while (i != end(program) && i->cmd != "}")
			{
				result.back().program.push_back(*i);
				++i;
			}

			if(i == end(program) || i->cmd != "}")
				throw ProgramException(string("Procedure") + result.back().name() + " is not full ( not enough } )", i->line);
		}
	}
	return result;
}

ListProgram process_cmds(
	const PreProgram& program,
	const std::map<std::string, command>& cmd_codes,
	const std::map<uint8_t, int>& cmd_to_args
)
{
	vector<pair<string, command>> cmd_codes_sort = { begin(cmd_codes), end(cmd_codes) };
	sort(begin(cmd_codes_sort), end(cmd_codes_sort), [](const pair<string, command>& lh, const pair<string, command>& rh) {
		return lh.first.size() > rh.first.size();
		});
	ListProgram result;
	for (const auto& [str,line] : program) {
		for (const auto& [cmd, code] : cmd_codes_sort) {
			if (cmd_to_args.at(code) > 0) {
				if (cmd.size() <= str.size() + 2 && cmd == str.substr(0, cmd.size())) {
					if (str[cmd.size()] != '(') throw ProgramException("not enough (", line);
					if (str[str.size() - 1] != ')') throw ProgramException("not enough )", line);

					auto arg = str.substr(cmd.size() + 1, str.size() - cmd.size() - 2);
					if (count(begin(arg), end(arg), ',') + 1 != cmd_to_args.at(code))
						throw ProgramException(cmd + ": " + string("wrong argument count"), line);

					result.push_back({ code,str_to_args(str.substr(cmd.size() + 1, str.size() - cmd.size() - 2)), line });
					break;
				}
			}
			else if (cmd_to_args.at(code) == 0) {
				if (cmd.size() <= str.size() + 2 && cmd == str.substr(0, cmd.size())) {
					if (str[cmd.size()] != '(') throw ProgramException("not enough (", line);
					if (str[str.size() - 1] != ')') throw ProgramException("not enough )", line);
					if (cmd.size() + 2 != str.size()) throw ProgramException(cmd + ": " + string("wrong argument count"), line);
					result.push_back({ code,str_to_args(str.substr(cmd.size() + 1, str.size() - cmd.size() - 2)), line });
					break;
				}
			}
			else if (cmd_to_args.at(code) == -1) {
				if (cmd.size() <= str.size() + 2 && cmd == str.substr(0, cmd.size())) {
					if (str[cmd.size()] != '(') throw ProgramException("not enough (", line);
					if (str[str.size() - 1] != ')') throw ProgramException("not enough )", line);

					result.push_back({ code,str_to_args(str.substr(cmd.size() + 1, str.size() - cmd.size() - 2)), line });
					break;
				}
			}
			else if (cmd_to_args.at(code) == -2) {
				if (cmd.size() <= str.size() && cmd == str.substr(0, cmd.size())) {
					result.push_back({ code,str_to_args(str.substr(cmd.size(), str.size() - cmd.size())), line });
					break;
				}
			}
		}
	}
	return result;
}

void process_args(
	ListProgram& program,
	const std::set<std::uint8_t>& cmds,
	const std::function<args_container(std::uint8_t, args_container)>& fnc
)
{
	for (auto& [cmd,args, line] : program) {
		for (auto pr_cmd : cmds) {
			if (cmd == pr_cmd) {
				args = fnc(cmd, args);
				break;
			}
		}
	}
}

void process_sizes(ListProgram& program)
{
	for (auto& [cmd, args, line] : program) {
		uint8_t arg_size = 0u;
		for (const auto& arg : args) {
			arg_size += arg.val.size() + 1;
		}
		arg_size -= (args.size()) ? 1 : 0;
		args.insert(begin(args), { string() + char(arg_size), true, true });
	}
}

void output_program(const ListProgram& program, std::ostream& out)
{
	for (const auto& [cmd, args, line] : program) {
		out << cmd;
		//size
		if(args.empty())
			throw CompileException("wow even size was not writed");
		out << args[0].val; 
		if (args.size() > 1) {
			for (size_t i = 1; i < args.size() - 1; ++i) {
				const auto& arg = args[i];
				if (arg.var_process != true || arg.type_process != true)
					throw CompileException("some process flags are not true (compiler error)");
				out << arg.val << ",";
			}
		
			const auto& arg = args[args.size() - 1];
			if (arg.var_process != true || arg.type_process != true)
				throw CompileException("some process flags are not true (compiler error)");
			out << arg.val;
		}
	}
}