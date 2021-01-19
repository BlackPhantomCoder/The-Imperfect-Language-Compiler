#include "Funcs.h"

args_container str_to_args(std::string arg_str)
{
    args_container result;
	auto pos = arg_str.find(',');
	while (pos != arg_str.npos) {
		result.push_back({ arg_str.substr(0, pos), false, false });
		arg_str.erase(0, pos + 1);
		pos = arg_str.find(',');
	}
	if (!arg_str.empty())result.push_back({arg_str.substr(0, pos), false, false });

    return result;
}
