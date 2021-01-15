#pragma once
#include <queue>
#include <stack>
#include <string>
#include <memory>
#include <vector>

#include "Expression.h"
//parsing expression to tokens 
std::queue<std::shared_ptr<Token>> tokenize(std::string input);
//parsing expression to postfix notation
std::vector<std::shared_ptr<Token>> to_postfix(const std::vector<std::shared_ptr<Token>>& expression);
//parsing expression to postfix notation to string with simbol-separator
std::string to_postfix(const std::string& expression, char simbol);
//parsing expression to postfix notation (with compution numeric expressions) to string with simbol-separator
std::string to_postfix_with_compute(const std::string& expression, char simbol);