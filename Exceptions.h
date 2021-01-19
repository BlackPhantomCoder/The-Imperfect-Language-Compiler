#pragma once
#include <stdexcept>
#include <string>
#include <iostream>

//Exception in program with line
class ProgramException : public std::invalid_argument
{
public:
	ProgramException(const std::string& message, size_t line) : std::invalid_argument(message + " at line " + std::to_string(line)) {}
};

//Exception in program without line 
class CompileException : public std::invalid_argument
{
public:
	CompileException(const std::string& message) : std::invalid_argument(message) {}
};

//Error on Compiler
class CompilerException : public CompileException
{
public:
	CompilerException(const std::string& message) : CompileException(message + " --> compiler error") {}
};