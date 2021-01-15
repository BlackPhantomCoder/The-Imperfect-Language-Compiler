#pragma once
#include <deque>
#include <string>
#include <list>
#include "Types.h"

//argument
struct arg_info {
	arg_info(const std::string& str, bool var, bool type) : val(str), var_process(var), type_process(type) {}
	std::string val = "";
	bool var_process = false;
	bool type_process = false;
};

//arguments
struct args_container : public std::deque<arg_info> {
	std::string to_string()const {
		if (empty()) return "";
		std::string result;
		for (const auto& arg : *this) {
			result += arg.val + ",";
		}
		result.erase(std::prev(std::end(result)));
	}
	args_container& operator += (const args_container& rh) {
		insert(std::end(*this), std::begin(rh), std::end(rh));
		return *this;
	}
};

//command string (with args) with line
struct cmd_str {
	std::string cmd = "";
	size_t line = 0;
};

//command with line and args
struct cmd_and_args {
	command cmd = command::nop;
	args_container args;
	size_t line = 0;
};

//
using PreProgram = std::deque<cmd_str>;
//
using ListProgram = std::list<cmd_and_args>;