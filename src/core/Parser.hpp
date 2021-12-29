#pragma once
#include "core/Common.hpp"
#include "core/Scanner.hpp"
#include "core/Types.hpp"


class Parser {
public:

	Parser(const std::vector<Token>& tokens);

	Status parse();

	const std::shared_ptr<Node>& tree() const {
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
	/*struct Result {

		Result(const Expression::Ptr& aexp) : exp(aexp), success(aexp != nullptr) {};

		Result(bool res) : exp(nullptr), success(res) {
			// Only for failure case.
			assert(!res);
		}
		
		std::shared_ptr<Expression> exp = nullptr;
		bool success = false;
	};*/

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
	std::shared_ptr<Node> _tree;
	std::string _failedMessage;
	long _position;
	long _tokenCount;
	long _failedToken = -1;
	bool _failed = false;
};
