#pragma once
#include "core/Common.hpp"
#include "core/Scanner.hpp"
#include "core/Types.hpp"


class Parser {
public:

	Parser(const std::vector<Token>& tokens);

	Status parse();

	const Expression::Ptr& tree() const {
		return _tree;
	}

private:

	bool valid() const;

	bool expect(Operator op) const;

	bool match(Operator op);

	bool match(std::initializer_list<Operator> ops);

	void advance();

	const Token& peek() const;

	const Token& previous() const;

	Operator previousOp() const;

	using Result = Expression::Ptr;

	Result statement();
	Result declaration();
	Result expression();
	Result ternary();
	Result boolOr();
	Result boolXor();
	Result boolAnd();
	Result bitOr();
	Result bitXor();
	Result bitAnd();
	Result equality();
	Result comparison();
	Result bitshift();
	Result term();
	Result factor();
	Result unary();
	Result power();
	Result member();
	Result terminal();


	std::vector<Token> _tokens;
	Expression::Ptr _tree;
	std::string _failedMessage;
	long _position;
	long _tokenCount;
	long _failedToken = -1;
	bool _failed = false;
	bool _parsingFunctionDeclaration = false;
};
