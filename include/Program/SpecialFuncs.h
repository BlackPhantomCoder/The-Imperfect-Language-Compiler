#pragma once
#include <cstdint>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include "Type/Types.h"
#include "Program/Program.h"

//processing math equations
void process_equations(PreProgram& program);
//processing arg_info
arg_info get_args(arg_info arg_inf);
//processing arguments by cmd code
args_container arg_process(std::uint8_t cmd, args_container args);
//processing if/else constructions
void process_ifs_else(ListProgram& program);
//processing variables (and arrays)
void process_variables(ListProgram& program);