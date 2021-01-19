#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <utility>
#include <cstdint>
#include <vector>
#include <filesystem>
#include <map>

#include "Types.h"
#include "ProgramFuncs.h"
#include "SpecialFuncs.h"
#include "TypesFuncs.h"
#include "Exceptions.h"
#include "Funcs.h"
#include "ProcedureFuncs.h"
using namespace std;

//reading names of program files
vector<pair<string, string>> get_programs_names(const string& path);
//compiling
void compile_program(istream& input, ostream& output);

int main() {

	const string input_path = "programs";
	const string ouput_path = "LAMP";
	vector<pair<string, string>> programs;
	try {
		programs = get_programs_names(input_path);
	}
	catch (...) {
		#ifndef _DEBUG
				cout << "get_programs_names error" << endl;
		#else 
			throw;
		#endif
	}


	for (const auto& [input, output] : programs) {
		try
		{
			ifstream input_file(input_path + "//" + input, ios::binary);
			ofstream output_file(ouput_path + "//" + output + ".p", ios::binary);
			compile_program(input_file, output_file);
		}
		#ifndef _DEBUG
			catch (const ProgramException& e)
			{
				cout << input << ": " << "compile failure" << endl;
				cout << "error in program" << endl;
				cout << e.what() << endl;
			}
			catch (const CompileException& e)
			{
				cout << input << ": " << "compile failure" << endl;
				cout << e.what() << endl;
			}
			catch (...) {
					cout << input << ": " <<  "program failure, unknown error" << endl;
			}
		#else
			catch (...) {
				throw;
			}
		#endif
	}
}

vector<pair<string, string>> get_programs_names(const string& path) {
	vector<pair<string, string>> result;
	for (auto& p : filesystem::directory_iterator(path)) {
		if (p.is_regular_file()) {
			auto input_name = p.path().filename().string();
			auto output_name = input_name;
			auto extension = p.path().extension().string();
			output_name.erase(output_name.find(extension), extension.size());
			result.push_back({ input_name, output_name });
		};
	}
	return result;
}

//parsing file, cleaning, parsing preprocedures
vector<PreProcedureStepOne> step_preprocedures_first(istream& input) {
	vector<PreProcedureStepOne> pre_procedures_one;
	auto pre_program = parse_program(input);
	clear_program(pre_program, cleaning);
	return get_PreProcedures(pre_program);
}

//searching for main procedure, processing procedure calls
map<size_t, PreProcedureStepTwo> step_preprocedures_second(vector<PreProcedureStepOne> pre_procedures_one) {
	auto main_iter = find_if(begin(pre_procedures_one), end(pre_procedures_one),
		[](const PreProcedureStepOne& rh) { return rh.name() == "main"; });

	if (main_iter == end(pre_procedures_one)) throw CompileException("main is not found");
	if (pre_procedures_one.size() > 1) {

		if (main_iter->id != 0) {
			auto zero_iter = find_if(begin(pre_procedures_one), end(pre_procedures_one),
				[](const PreProcedureStepOne& rh) { return rh.id == 0; });
			if (zero_iter == end(pre_procedures_one))
				throw CompilerException("no zero-id procedure");
			swap(zero_iter->id, main_iter->id);
		}

		sort(begin(pre_procedures_one), end(pre_procedures_one),
			[](const PreProcedureStepOne& lh, const PreProcedureStepOne& rh) { return lh.name() > rh.name(); });

		for (auto& lh : pre_procedures_one) {
			for (const auto& rh : pre_procedures_one) {
				process_procedure_calls(lh.program, rh);
			}
		}
	}

	map<size_t, PreProcedureStepTwo> pre_procedures_two;
	for (const auto& p : pre_procedures_one) {
		pre_procedures_two.insert({ p.id, next_step(p) });
	}
	return pre_procedures_two;
}

//processing procedure args, vars, cmds + cmd's args, if/else construction
map<size_t, Procedure> step_procedures(map<size_t, PreProcedureStepTwo> pre_procedures_two) {
	map<size_t, Procedure> procedures;
	vector<vector<cmd_and_args>> args;
	for (auto& [id,p] : pre_procedures_two) {
		args.push_back(process_vars_and_call_args(p));
	}

	for (size_t i = 0; i < pre_procedures_two.size(); ++i) {
		procedures.insert({ i, compile_procedure(pre_procedures_two.at(i), args[i]) });
	}
	return procedures;
}

void compile_program(istream& input, ostream& output)
{
	auto null = step_preprocedures_first(input);
	auto first = step_preprocedures_second(move(null));

	auto procedures = step_procedures(move(first));
	auto procedure_table = gen_procedure_table(procedures);

	output_program(procedure_table, output);
	for (const auto& [id, p] : procedures) {
		output_program(p.program, output);
	}
}