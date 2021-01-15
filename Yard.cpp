#include "Yard.h"
#include <algorithm>
#include <set>
#include <stdexcept>
#include <stack>
using namespace std;

void clear_str(std::string& str, const set<char>& simbols) {
    for (auto symbol : simbols) {
        auto new_end = remove(begin(str), end(str), symbol);
       if(new_end != end(str)) str.erase(new_end, end(str));
    }
}

bool is_numb(char ch) {
    return (ch >= '0' && ch <= '9');
}

bool is_simbol(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

std::queue<std::shared_ptr<Token>> tokenize(std::string input)
{
    std::queue<std::shared_ptr<Token>> result;
    
    clear_str(input, {' ', char(9), '\t', '\r', '\n'});
    
    set<char> opertors = { '+', '*', '^', '/', '%' };
    char minus = '-';
    auto i = begin(input);
    while (i != end(input)) {
        char& ch = *i;
        if (opertors.count(ch)) {
            TokenOperator::op_type type;
            switch (ch)
            {
            case '+':
                type = TokenOperator::op_type::plus;
                break;
            case '*':
                type = TokenOperator::op_type::mlt;
                break;
            case '^':
                type = TokenOperator::op_type::pow;
                break;
            case '/':
                type = TokenOperator::op_type::div;
                break;
            case '%':
                type = TokenOperator::op_type::mod;
                break;
            }
            result.push(make_shared<TokenOperator>(type));
        }
        else {
            if (is_numb(ch)) {
                string num;
                while (i != end(input) && is_numb(*i)) {
                    num += *i;
                    ++i;
                }
                result.push(make_shared<TokenNum>(num));
                continue;
            }
            else {
                if (ch == minus) {
                    if(next(i) == end(input))throw invalid_argument("");
                    if (result.empty() || result.back()->get_type() == Token::token_type::open_braket) {
                        if (!is_simbol(ch))throw invalid_argument("");
                        string name;
                        while (i != end(input) && opertors.count(*i) == 0) {
                            name += *i;
                            ++i;
                        }
                        result.push(make_shared<TokenVar>(name));
                        continue;
                    }
                    else
                    {
                        result.push(make_shared<TokenOperator>(TokenOperator::op_type::minus));
                    }
                }
                else if (is_simbol(ch)) {
                    string name;
                    while (i != end(input) && opertors.count(*i) == 0 && *i != '-') {
                        name += *i;
                        ++i;
                    }
                    result.push(make_shared<TokenVar>(name));
                    continue;
                }
                else {
                    if (ch == '(') {
                        result.push(make_shared<Token>(Token::token_type::open_braket));
                    }
                    else if(ch == ')') {
                        result.push(make_shared<Token>(Token::token_type::close_braket));
                    }
                    else {
                        throw invalid_argument("");
                    }
                   
                }
            }
        }
        ++i;
    }
    return result;
}
#include <list>

bool is_operand(Token::token_type type) {
    return type == Token::token_type::num || type == Token::token_type::var;
}

vector<std::shared_ptr<Token>> to_postfix(const std::vector<std::shared_ptr<Token>>& expression)
{
    stack<std::shared_ptr<Token>> opstack;
    vector<std::shared_ptr<Token>> output;

    for (const auto& op : expression) {
        if (is_operand(op->get_type())) {
            output.push_back(op);
        }
        else if(op->get_type() == Token::token_type::open_braket){
            opstack.push(op);
        }
        else if (op->get_type() == Token::token_type::close_braket) {
            while (opstack.top()->get_type() != Token::token_type::open_braket) {
                output.push_back(move(opstack.top()));
                opstack.pop();
            }
            opstack.pop();
        }
        else if (op->get_type() == Token::token_type::op) {
            while
                (
                    !opstack.empty()
                    &&
                    opstack.top()->get_type() == Token::token_type::op
                    &&
                    (*static_pointer_cast<TokenOperator>(opstack.top()) >= *static_pointer_cast<TokenOperator>(op))
                )
            {
                output.push_back(move(opstack.top()));
                opstack.pop();
            }
            opstack.push(op);
        }
    }

    while (!opstack.empty()) {
        auto buf = opstack.top();
        opstack.pop();
        output.push_back(buf);
    }
    return output;
}

string to_postfix(const string& expression, char simbol) {
    string result;
    auto tokens = tokenize(expression);
    vector<shared_ptr<Token>> expr;
    expr.reserve(tokens.size());
    while (!tokens.empty()) {
        expr.push_back(move(tokens.front()));
        tokens.pop();
    }

    auto postfix = to_postfix(expr);
    for (const auto& token : postfix) {
        result += string(*token) + simbol;
    }
    result.erase(prev(end(result)));
    return result;
}
#include <sstream>
long to_numb(const string& str) {
    long result;
    istringstream s(str);
    s >> result;
    return result;
}
#include <iostream>

std::string to_postfix_with_compute(const std::string& expression, char simbol)
{
    string result;
    auto tokens = tokenize(expression);
    vector<shared_ptr<Token>> expr;
    expr.reserve(tokens.size());
    while (!tokens.empty()) {
        expr.push_back(move(tokens.front()));
        tokens.pop();
    }

    auto postfix = to_postfix(expr);

    stack<shared_ptr<Token>> args;

    auto i = begin(postfix);
    while (i != end(postfix)) {
        const auto& token = *i;

        if (token->get_type() == Token::token_type::num) {
            args.push(token);
        }
        else if(token->get_type() == Token::token_type::op) {
            
            long arg_2 = to_numb(static_pointer_cast<TokenNum>(args.top())->get_num());
            args.pop();
            long arg_1 = to_numb(static_pointer_cast<TokenNum>(args.top())->get_num());
            args.pop();

            long res_arg;
            switch (static_pointer_cast<TokenOperator>(token)->get_op_type())
            {
            case TokenOperator::op_type::mlt:
                res_arg = arg_1 * arg_2;
                break;
            case TokenOperator::op_type::div:
                res_arg = arg_1 / arg_2;
                break;
            case TokenOperator::op_type::plus:
                res_arg = arg_1 + arg_2;
                break;
            case TokenOperator::op_type::mod:
                res_arg = arg_1 % arg_2;
                break;
            case TokenOperator::op_type::pow:
                res_arg = arg_1 ^ arg_2;
                break;
            case TokenOperator::op_type::minus:
                res_arg = arg_1 - arg_2;
                break;
            }
            args.push(make_shared<TokenNum>(to_string(res_arg)));
        }
        else {
            vector<shared_ptr<Token>> new_postfix;
            while (!empty(args)) {
                new_postfix.insert(begin(new_postfix),move(args.top()));
                args.pop();
            }
            new_postfix.insert(end(new_postfix), i, end(postfix));
            postfix = new_postfix;
            break;
        }
        ++i;
    }
    if (!empty(args)) {
        postfix = { args.top() };
    }

    for (const auto& token : postfix) {
        result += string(*token) + simbol;
    }
    result.erase(prev(end(result)));
    return result;
}
