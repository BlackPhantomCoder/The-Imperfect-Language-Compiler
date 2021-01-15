#include "TypesFuncs.h"
#include <iostream>

using namespace std;

size_t type_to_cmd_str_size(types type) {
	switch (type)
	{
	case types::BYTE:
		return 4;
		break;
	case types::DBYTE:
		return 5;
		break;
	case types::LOG:
		return 4;
		break;
	case types::CG:
		return 2;
		break;
	case types::CR:
		return 2;
		break;
	case types::CB:
		return 2;
		break;
	case types::OP:
		return 1;
		break;
	}
	throw;
}


types get_type_new(char ch)
{
	return types(ch);
}

types get_type(const string& str)
{

	const static map<string, types> str_to_type = {
		{"byte", types::BYTE},
		{"dbyte", types::DBYTE},
		{"bool", types::LOG},
		{"CR", types::CR},
		{"CG", types::CG},
		{"CB", types::CB},
		{"+", types::OP},
		{"-", types::OP},
		{"/", types::OP},
		{"*", types::OP},
		{"^", types::OP},
		{"%", types::OP}
	};
	for (const auto& [name, type] : str_to_type) {
		if (str.size() >= name.size() && str.substr(0, name.size()) == name) {
			return type;
		}
	}

	if (str.size() > 3)return types::NUM_DBYTE;
	if (str.size() == 3 && str[0] > '2')return types::NUM_DBYTE;
	if (str.size() == 3 && str[0] == '2' && str[1] > '5')return types::NUM_DBYTE;
	if (str.size() == 3 && str[0] == '2' && str[1] == '5' && str[2] > '5')return types::NUM_DBYTE;

	return types::NUM_BYTE;
}

uint16_t to_dbyte(const string& str)
{
	uint16_t result = 0u;
	uint8_t size = str.length() - 1;
	for (int i = size; i >= 0; --i) {
		uint16_t buf = to_numb(str[i]);
		for (uint8_t j = 0; j < size - i; ++j) {
			buf *= 10;
		}
		result += buf;
	}
	return result;
}

std::string byte_to_str(std::uint8_t b)
{
	return std::string() + char(b);
}

std::string dbyte_to_str(std::uint16_t db)
{
	string result;
	result += (db >> 8);
	result += uint8_t((db << 8) >> 8);
	return result;
}

uint16_t to_dbyte_new(const string& str)
{
	return (uint16_t(str[0]) << 8) + uint16_t(uint8_t(str[1]));
}

uint8_t to_numb(char ch)
{
	if (ch < '0' || ch > '9') throw invalid_argument("it is not a number");
	return ch - '0';
}

string get_next_arg_ref(string& str, char symbol)
{
	string result;
	uint8_t i = 0;
	while (i != str.size() && str[i] != symbol) {
		result += str[i];
		++i;
	}
	str.erase(0, (i != str.size()) ? i + 1 : i);
	return result;
}

std::string num_to_str(std::uint16_t num)
{
	std::string result;
	if (num > 255) {
		result += char(types::NUM_DBYTE);
		result += dbyte_to_str(num);
	}
	else {
		result += char(types::NUM_BYTE);
		result += byte_to_str(num);
	}
	return result;
}
