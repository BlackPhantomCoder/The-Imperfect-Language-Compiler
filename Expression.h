#pragma once
#include <string>
#include <map>

//
class Token
{
public:
	enum class token_type {
		var, op, num, open_braket, close_braket
	};
public:
	Token(token_type type) :t_type(type) {}
	token_type get_type() { return t_type; }
	virtual operator std::string() {
		std::string result;
		switch (t_type)
		{
		case Token::token_type::var:
			result += "var";
			break;
		case Token::token_type::op:
			result += "op";
			break;
		case Token::token_type::num:
			result += "num";
			break;
		case Token::token_type::open_braket:
			result += "(";
			break;
		case Token::token_type::close_braket:
			result += ")";
			break;
		}
		return result;
	}
private:
	token_type t_type;
};

//Var Token
class TokenVar : public Token{
public:
	TokenVar(std::string name) : Token(Token::token_type::var), t_name(name) {}
	std::string get_name() { return t_name; }
	operator std::string() override {
		return t_name;
	}
private:
	std::string t_name;
};

//Operator Token
class TokenOperator : public Token {
public:
	enum class op_type {
		plus,
		minus,
		div,
		mod ,
		mlt,
		pow,
	};
public:
	TokenOperator(op_type type) : Token(Token::token_type::op), t_type(type) {}
	op_type get_op_type() { return t_type; }
	bool operator>(const TokenOperator& rh) {
		return t_priors.at(t_type) > t_priors.at(rh.t_type);
	}
	bool operator<(const TokenOperator& rh) {
		return t_priors.at(t_type) < t_priors.at(rh.t_type);
	}
	bool operator==(const TokenOperator& rh) {
		return t_priors.at(t_type) == t_priors.at(rh.t_type);
	}
	bool operator>=(const TokenOperator& rh) {
		return t_priors.at(t_type) >= t_priors.at(rh.t_type);
	}
	bool operator<=(const TokenOperator& rh) {
		return t_priors.at(t_type) <= t_priors.at(rh.t_type);
	}
	operator std::string() override{
		std::string result;
		switch (t_type)
		{
		case TokenOperator::op_type::plus:
			result += '+';
			break;
		case TokenOperator::op_type::minus:
			result += '-';
			break;
		case TokenOperator::op_type::div:
			result += '/';
			break;
		case TokenOperator::op_type::mod:
			result += '%';
			break;
		case TokenOperator::op_type::mlt:
			result += '*';
			break;
		case TokenOperator::op_type::pow:
			result += '^';
			break;
		}
		return result;
	}
private:
	op_type t_type;
	const std::map<op_type, size_t> t_priors = std::map<op_type, size_t>{
		{op_type::pow,2},
		{op_type::div,2},
		{op_type::mod,2},
		{op_type::mlt,2},
		{op_type::plus,1},
		{op_type::minus,1}
	};
};

//Number Token
class TokenNum : public Token {
public:
	TokenNum(std::string num) : Token(Token::token_type::num), t_num(num) {}
	std::string get_num() { return t_num; }
	operator std::string() override {
		return t_num;
	}
private:
	std::string t_num;
};