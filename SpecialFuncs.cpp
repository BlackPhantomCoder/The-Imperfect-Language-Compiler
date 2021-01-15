#include "SpecialFuncs.h"

#include <algorithm>
#include <list>
#include <map>
#include <functional>
#include <iostream>

#include "Yard.h"
#include "TypesFuncs.h"
#include "Exceptions.h"
using namespace std;

void process_equations(PreProgram& program)
{
	for (auto& [str, line] : program) {
		size_t pos = str.find('=');
		if (pos != str.npos) {
			if (str[pos - 1] == ',' || str[pos + 1] == ',')continue;
			auto left = str.substr(0, pos);
			auto right = str.substr(pos + 1, str.size());
			str = string("set(") + left + "," + right + ")";
		}
	}
}

arg_info get_args(arg_info arg_inf)
{
		arg_info arg_result{ "", arg_inf.var_process, true };
		auto& arg = arg_inf.val;
		while (!arg.empty()) {
			const auto type = get_type(arg);
			if (type == types::NUM_BYTE) {
				arg_result.val += char(type);
				arg_result.val += char(to_dbyte(arg));
				break;
			}
			if (type == types::NUM_DBYTE) {
				arg_result.val += char(type);
				arg_result.val += dbyte_to_str(to_dbyte(arg));
				break;
			}
			switch (type)
			{
			case types::BYTE:
			case types::LOG:
				arg.erase(0, type_to_cmd_str_size(type));
				//arg.erase(0, 1);
				break;
			case types::DBYTE:
			case types::CG:
			case types::CR:
			case types::CB:
				arg.erase(0, type_to_cmd_str_size(type));
				//arg.erase(0, 2);
				break;
			case types::OP:
				arg_result.val += char(type);
				switch (arg[0])
				{
				case '+':
					arg_result.val += char(operations::plus);
					break;
				case '-':
					arg_result.val += char(operations::minus);
					break;
				case '*':
					arg_result.val += char(operations::mlt);
					break;
				case '/':
					arg_result.val += char(operations::div);
					break;
				case '%':
					arg_result.val += char(operations::mod);
					break;
				case '^':
					arg_result.val += char(operations::pow);
					break;
				}
				arg.erase(0, type_to_cmd_str_size(type));
				//arg.erase(0, 1);
				continue;
				break;
			}
			arg_result.val += char(type);
		}
		return arg_result;
}
string get_args_string(string str) {
	return get_args({ str,  false, false }).val;
}
bool process_exceptions(arg_info& arg_inf);

bool process_exceptions_str(string& str) {
	auto buf = arg_info{ str,  false, false };
	auto result = process_exceptions(buf);
	str = buf.val;
	return result;
}

bool process_exceptions(arg_info& arg_inf)
{
	auto& str = arg_inf.val;
	static auto operators = {'-', '+', '*', '%', '/','^'};
	if (str.find('[') == str.npos) {
		for (auto op : operators) {
			if (str.find(op) != str.npos) {
				str = to_postfix_with_compute(str, '|');
				auto count = std::count(begin(str), end(str), '|') + 1;
				if (count == 1) {
					str = get_args_string(str);
					return true;
				}
				string result_exception = get_args_string(to_string(count));// +",";
				while (!str.empty()) {
					auto arg = get_next_arg_ref(str, '|');
					auto post_arg = get_args_string(arg);
					result_exception += get_args_string(to_string(post_arg.size())) + post_arg;// +'|';
				}
				//result_exception.erase(prev(end(result_exception)));
				str = string("") + char(types::E) + result_exception;
				return true;
			}
		}
	}
	else {
		static function<size_t(size_t, const string&)> f = [](size_t pos, const string& str) {
			if (str[pos] == '[') pos = f(pos + 1, str) + 1;
			if (str[pos] == ']') return pos;
			return f(pos + 1, str);
		};

		map<string, pair<string, string>> t_name_to_array;
		while (str.find_first_of('[') != str.npos) {
			auto pos_beg = str.find_first_of('[');
			auto pos_end = f(pos_beg + 1, str);
			auto arg = str.substr(pos_beg + 1, pos_end - pos_beg - 1);
			if (!process_exceptions_str(arg)) arg = get_args_string(arg);

			string name = "A" + to_string(t_name_to_array.size());
			str.erase(pos_beg, pos_end - pos_beg + 1);

			string type;
			{
				auto pos = pos_beg - 1;
				while (str[pos] != '\n'){
					type.insert(begin(type), str[pos]);
					--pos;
				}
				str.erase(pos, type.size() + 1);
				pos_beg = pos;
			}
			


			str.insert(pos_beg, name);
			
			t_name_to_array[name] = { arg, type };
		}

		
		for (auto op : operators) {
			if (str.find(op) != str.npos) {
				str = to_postfix_with_compute(str, '|');
				string result_exception = get_args_string(to_string(std::count(begin(str), end(str), '|') + 1));// +",";
				while (!str.empty()) {
					bool expr = false;
					auto arg = get_next_arg_ref(str, '|');
					for (const auto& [name,arr] : t_name_to_array) {
						if (arg == name) {
							arg = get_args_string(arr.second) +  arr.first;
							expr = true;
							break;
						}
					}
					if (!expr) {
						auto post_arg = get_args_string(arg);
						result_exception += get_args_string(to_string(post_arg.size())) + post_arg;// +'|';
					}
					else {
						result_exception += get_args_string(to_string(arg.size())) + arg;// +'|';
					}
					
				}
				//result_exception.erase(prev(end(result_exception)));
				str = string("") + char(types::E) + result_exception;
				return true;
			}
		}
		if (t_name_to_array.size() == 1) {
			str = get_args_string(t_name_to_array[str].second) + t_name_to_array[str].first;
			return true;
		}
		else {
			throw "";
		}
		
	}
	
	return false;
}


args_container arg_process(uint8_t cmd, args_container args) {
	args_container result;
	switch (cmd)
	{
	case command::set:
	case command::swap:
	case command::random:
	case command::for_:
	case command::while_:
	case command::pause:
	case command::bool_:
	case command::byte_:
	case command::dbyte_:
	case command::cset:
	case command::ccopy:
	case command::cswap:
	case command::ccopyall:
	case command::procedure_call:
		for (auto& arg : args) {
			if (process_exceptions(arg))result.push_back(arg);
			else result.push_back(get_args(arg));
		}
		break;
	case command::if_:
	{
		//	numeric(type), >/</=, numeric(type)

		if (process_exceptions(args[0]))result.push_back(args[0]);
		else result.push_back(get_args(args[0]));

		result.push_back(args[1]);

		if (process_exceptions(args[2]))result.push_back(args[2]);
		else result.push_back(get_args(args[2]));
	}
	break;

	case command::draw:
	case command::var:
	case command::eif:
	case command::else_:
	case command::eelse:
	case command::evar:
	case command::ewhile:
	case command::efor:
	case command::end_procedure:
		result = move(args);
		break;
	default:
		throw CompileException("unprocessec cmd : code(" + to_string(cmd) + ")");
	}
	for (auto& arg : result) {
		arg.type_process = true;
	}
	return result;
}


void operator += (ListProgram::iterator& lh, size_t rh) {
	for (size_t i = 0; i < rh; ++i)++lh;
}

pair<uint16_t, uint16_t> process_ifs_r(
	ListProgram& program,
	ListProgram::iterator beg
);

pair<uint16_t, uint16_t> process_elses_r(
	ListProgram& program,
	ListProgram::iterator beg
)
{
	uint16_t count_cmd = 0;
	uint16_t count = 0;
	for (auto iter = next(beg); iter != end(program); ++iter) {
		if (iter->cmd == command::else_) {
			auto buf = process_elses_r(program, iter);
			count += buf.second - buf.first;
			count_cmd += buf.first;
			iter += buf.first-1;
			continue;
		}
		else if (iter->cmd == command::eelse) {
			program.erase(iter);
			//beg->second += uint8_t(types::NT);
			//beg->second += dbyte_to_str(count + count_cmd);
			//beg->second[0] += 3;
			auto buf = get_args_string(to_string(count + count_cmd));
			beg->args.push_back({ buf, true, true });
			beg->args[0].val[0] += buf.size();
			//count += iter->second.size();
			//++count_cmd;
			break;
		}
		else if(iter->cmd == command::if_){
			auto res = process_ifs_r(program, iter);
			count_cmd += res.first;
			count += res.second - res.first;
			iter += res.first-1;
			continue;
		}
		count += iter->args.size();
		++count_cmd;
	}
	count_cmd += 1;
	count += beg->args.size();
	count += count_cmd;
	return { count_cmd,count };
}

pair<uint16_t, uint16_t> process_ifs_r(
	ListProgram& program,
	ListProgram::iterator beg
)
{
	uint16_t count_cmd = 0;
	uint16_t count = 0;
	for (auto iter = next(beg); iter != end(program); ++iter) {
		if (iter->cmd == command::if_) {
			auto buf = process_ifs_r(program, iter);
			count += buf.second - buf.first;
			count_cmd += buf.first;
			iter += buf.first-1;
			continue;
		}
		else if (iter->cmd == command::eif) {
			auto next_it = next(iter);
			program.erase(iter);
			auto buf = get_args_string(to_string(count + count_cmd));
			beg->args.push_back({ buf, true, true });
			beg->args[0].val[0] += 1 + buf.size();
			/*beg->args += buf;
			beg->args[0] += 1 + buf.size();*/
	

			if (next_it != end(program) && next_it->cmd == command::else_) {
				auto res = process_elses_r(program, next_it);
				count_cmd += res.first;
				count += res.second - res.first;
			}
			break;
		}
		count += iter->args.size();
		++count_cmd;
	}
	count_cmd += 1;
	count += beg->args.size();
	count += count_cmd;
	return { count_cmd,count };
}

void process_ifs_else(ListProgram& program)
{
	auto iter = find_if(begin(program), end(program), [](const cmd_and_args& rh) {return rh.cmd == command::if_; });
	while (iter != end(program)) {
		iter += process_ifs_r(program, iter).first;
		iter = find_if(next(iter), end(program), [](const cmd_and_args& rh) {return rh.cmd == command::if_; });
	}
	program.erase(remove_if(begin(program), end(program), [](const cmd_and_args& rh) {return rh.cmd == command::eif; }), end(program));
}

void process_variable(
	ListProgram& program,
	ListProgram::iterator beg,
	string name,
	string repl_name
) 
{
	for (auto iter = beg; iter != end(program); ++iter) {
		size_t buf;
		for (auto& arg : iter->args) {
			while ((!arg.var_process) && (buf = arg.val.find(name)) != arg.val.npos) {
				arg.val.erase(buf, name.size());
				arg.val.insert(buf, repl_name);
			}
		}
		
	}
}

struct arr {
	string name;
	uint8_t offset;
	string type_name;
};

//[arg]...
string get_arr_arg(size_t beg, const string& str) {
	static function<size_t(size_t, const string&)> f = [](size_t pos, const string& str) {
		if (str[pos] == '[') pos = f(pos + 1, str) + 1;
		if (str[pos] == ']') return pos;
		return f(pos + 1, str);
	};
	auto end_pos = f(beg + 1, str);
	auto arg = str.substr(beg + 1, end_pos - beg - 1);
	return arg;
}


void process_array(
	ListProgram& program,
	ListProgram::iterator beg,
	const arr& a
)
{
	vector<string> add_vars;
	for (auto iter = beg; iter != end(program); ++iter) {
		auto& args = iter->args;
		static function<void(string&, const arr&)> f = [](string& str, const arr& a) {
			if (str.find(a.name) != str.npos) {
				size_t buf = str.find(a.name);
				str.erase(buf, a.name.size());
				str.insert(buf, "\n" + a.type_name);
				buf += a.type_name.size() + 1;
				auto arg = get_arr_arg(buf, str);
				str.erase(buf, arg.size() + 2);
				f(arg, a);
				str.insert(buf, "\t" + arg + ((a.offset) ? "+" + to_string(a.offset) : "") + "\r");
			}
		};
		bool found = false;
		for (auto& arg : args) {
			if (arg.val.find(a.name) != arg.val.npos) {
				f(arg.val, a);
				found = true;
			}
		}
		if (found)--iter;
	}
}



void process_variables(ListProgram& program)
{
	auto ibeg = find_if(begin(program), end(program), [](const cmd_and_args& rh) { return rh.cmd == command::var; });
	auto iend = find_if(begin(program), end(program), [](const cmd_and_args& rh) { return rh.cmd == command::evar; });

	vector<pair<string, string>> names;

	vector<arr> arrays;
	map <command, std::pair<uint16_t, size_t>> types;
	if (ibeg != iend) {
		for (auto iter = next(ibeg); iter != iend; ++iter) {
			auto buf = iter->args;
			size_t count = 0;
			string type_name;
			command type;
			size_t line = iter->line;
			switch (iter->cmd)
			{
			case command::byte_:
				type_name = string("") + char(0);
				type = command::byte_;
				break;
			case command::dbyte_:
				type_name = string("") + char(1);
				type = command::dbyte_;
				break;
			case command::bool_:
				type_name = string("") + char(2);
				type = command::bool_;
				break;
			}
			types[type].second = line;
			for(const auto& arg_info : buf){
				const auto& arg = arg_info.val;
				if (arg.find('[') != arg.npos) {

					arr a;
					a.name = arg.substr(0, arg.find('['));
					a.offset = types[type].first;
					a.type_name = type_name;

					auto b = arg.find('[') + 1;
					auto e = arg.find(']');
					types[type].first += to_dbyte(arg.substr(b, e - b));
					arrays.push_back(a);

				}
				else {
					names.push_back({ arg, type_name + to_string(types[type].first) });
					++types[type].first;
				}

			}
		}




		sort(begin(names), end(names), 
			[](const pair<string, string>& lh, const pair<string, string>& rh) {return lh.first.size() > rh.first.size(); });

		for (auto p : names) {
			process_variable(program, next(iend), p.first, p.second);
		}




		sort(begin(arrays), end(arrays), [](const arr& lh, const arr& rh) {return lh.name.size() > rh.name.size(); });
		for (auto a : arrays) {
			process_array(program, next(iend), a);
		}

		for (auto& [cmd, args_arr, line] : program) {
			for (auto& arg_info : args_arr) {
				auto& arg = arg_info.val;

				size_t pos = arg.find('\t');
				while (pos != arg.npos) {
					arg[pos] = '[';
					pos = arg.find('\t');
				}

				pos = arg.find('\r');
				while (pos != arg.npos) {
					arg[pos] = ']';
					pos = arg.find('\r');
				}
			}
			
		}


		for (auto& [cmd, args_arr, line] : program) {
			for (auto& arg_info : args_arr) {
				auto& arg = arg_info.val;

				size_t pos = arg.find(char(0));
				while (pos != arg.npos) {
					arg.erase(pos, 1);
					arg.insert(pos, "byte");
					pos = arg.find(char(0));
				}


				pos = arg.find(char(1));
				while (pos != arg.npos) {
					arg.erase(pos, 1);
					arg.insert(pos, "dbyte");
					pos = arg.find(char(1));
				}

				pos = arg.find(char(2));
				while (pos != arg.npos) {
					arg.erase(pos, 1);
					arg.insert(pos, "bool");
					pos = arg.find(char(2));
				}
			}
		}

		if (!types.empty()) {
			program.erase(next(ibeg), iend);
		}


		{
			ibeg = find_if(begin(program), end(program), [](const cmd_and_args& rh) { return rh.cmd == command::var; });
			ListProgram cmds;
			for (const auto& [cmd, arg] : types) {
				cmds.push_back({ cmd, std::deque<arg_info>{ {to_string(arg.first), true, false }}, arg.second });
			}
			program.insert(next(ibeg), begin(cmds), end(cmds));
		}
	}		
	for (auto& [cmd, args, line] : program) {
		for (auto& arg : args) {
			arg.var_process = true;
		}
	}
}
