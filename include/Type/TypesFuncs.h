#pragma once
#include <cstdint>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include "Type/Types.h"

//return count for each types
size_t type_to_cmd_str_size(types type);
//8 bit numeric from char
std::uint8_t to_numb(char ch);
//string to types
types get_type(const std::string& str);
//16 bit numeric from string
std::uint16_t to_dbyte(const std::string& str);
//8 bit numeric to string
std::string byte_to_str(std::uint8_t b);
//16 bit numeric to string
std::string dbyte_to_str(std::uint16_t db);
//splitting string byt symbol with remove
std::string get_next_arg_ref(std::string& str, char symbol = ',');
//16 bit/8bit  numeric to string with type byte
std::string num_to_str(std::uint16_t num);