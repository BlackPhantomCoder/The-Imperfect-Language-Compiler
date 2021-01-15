#pragma once
#include <map>
#include <set>
#include <string>
#include <utility>
#include <cstdint>
#include <numeric>

enum class types {
	BYTE = 0,
	DBYTE,
	LOG,
	CG,
	CR,
	CB,
	E,
	OP,
	NUM_BYTE,
	NUM_DBYTE
};

enum class operations {
	plus = 0,
	minus,
	pow,
	mlt,
	div,
	mod
};

namespace commands {
	// 9-13,  - not in use
	enum command : uint8_t {
		var = 0,
		evar = 1,

		procedure = 2,
		end_procedure = 3,
		procedure_call = 4,

		set = 5,
		swap = 6,
		random = 7,
		if_ = 8,
		for_ = 14,
		while_ = 15,
		pause = 16,
		draw = 17,
		eif = 18,
		else_ = 19,
		eelse = 20,
		efor = 21,
		ewhile = 22,

		bool_ = 23,
		byte_ = 24,
		dbyte_ = 25,

		cset = 26,
		ccopy = 27,
		cswap = 28,
		ccopyall = 29,


		stop = 254,
		nop = 255,
	};
};
using commands::command;

//string commad name with command
const std::map<std::string, command> cmd_and_name =
{
	{ "var",command::var },
	{ "evar",command::evar },
	{ "set",command::set },
	{ "swap",command::swap },
	{ "random",command::random },
	{ "if",command::if_ },
	{ "for",command::for_ },
	{ "while",command::while_ },
	{ "pause",command::pause },
	{ "draw",command::draw },
	{ "eif",command::eif },
	{ "else",command::else_ },
	{ "eelse",command::eelse },
	{ "efor",command::efor },
	{ "ewhile",command::ewhile },
	{ "bool",command::bool_ },
	{ "byte",command::byte_ },
	{ "dbyte",command::dbyte_ },
	{ "cset",command::cset },
	{ "ccopy",command::ccopy },
	{ "cswap",command::cswap },
	{ "ccopyall",command::ccopyall },
	{ "procedure_call", command::procedure_call },
	{ "end_procedure",command::end_procedure }
};

//set of commands
const std::set<std::string> cmd = []() -> auto{
	std::set<std::string> result;
	for (const auto& [name, cmd] : cmd_and_name) result.insert(name);
	return result;
}();

//set of commands codes
const std::set<std::uint8_t> cmd_codes = []() -> auto{
	std::set<std::uint8_t> result;
	for (const auto& [name, cmd] : cmd_and_name) result.insert(cmd);
	return result;
}();

//commad code with count args (-1 == not fixed)
const std::map<std::uint8_t, int> cmd_to_args = {
	{command::var, 0},
	{command::evar ,    0},
	{command::set ,	 2},
	{command::swap,		 2},
	{command::random ,	 3},
	{command::if_ ,		 3},
	{command::for_ ,	 3},
	{command::while_ ,	 1},
	{command::pause,	 1},
	{command::draw ,	 0},
	{command::eif ,	 0},
	{command::else_  ,	 0},
	{command::eelse  ,	 0},
	{command::efor ,	 0},
	{command::ewhile ,	 0},
	{command::bool_ ,	 0},
	{command::byte_ ,		 0},
	{command::dbyte_  ,	 0},
	{command::cset ,	 4},
	{command::ccopy,	 2},
	{command::cswap,		 2},
	{command::ccopyall,		 1},
	{command::procedure_call, -1},
	{command::end_procedure,		 0}
};

//type string with type cmd (sorted by greater)
const std::map<std::string, command, std::greater<std::string>> type_and_name = {
	{ "bool",command::bool_ },
	{ "byte",command::byte_ },
	{ "dbyte",command::dbyte_ },
};

//symbols for clean
const std::set<char> cleaning{
	' ',
	'	',
	'\r',
	'\t'
};