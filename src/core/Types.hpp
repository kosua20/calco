#pragma once
#include "core/Common.hpp"
#include <unordered_map>
#include <glm/gtc/constants.hpp>

enum class Operator {
	OpenParenth, CloseParenth, Plus, Minus, Product, Divide, Power, Modulo, Assign, ShiftLeft, ShiftRight, BitOr, BitAnd, BitNot, BitXor, BoolOr, BoolAnd, BoolNot, BoolXor, QuestionMark, Colon, LessThan, GreaterThan, LessThanEqual, GreaterThanEqual, Equal, Different, Comma, Dot
};

static const std::unordered_map<std::string, double> MathConstants = {
	{"pi", glm::pi<double>() },
	{"deg",  glm::pi<double>() / 180.0 },
};

inline std::string OperatorString(Operator op){
	static const std::vector<std::string> opStrs = {
		"(", ")", "+", "-", "*", "/", "^", "%", "=", "<<", ">>", "|", "&", "~", "@", "||", "&&", "!", "@@", "?", ":", "<", ">", "<=", ">=", "==", "!=", ",", "."
	};
	return opStrs[uint(op)];
}

struct Value {

	double scalar;
	std::string str;

	Value(double val) : scalar(val){}

	Value(const std::string& val) : str(val){}

};


class Expression;
class Unary;
class Binary;
class Ternary;
class Member;
class Literal;
class Variable;
class FunctionCall;

class TreeVisitor {
public:
	virtual Value process(const Expression& exp) = 0;
	virtual Value process(const Unary& exp) = 0;
	virtual Value process(const Binary& exp) = 0;
	virtual Value process(const Ternary& exp) = 0;
	virtual Value process(const Member& exp) = 0;
	virtual Value process(const Literal& exp) = 0;
	virtual Value process(const Variable& exp) = 0;
	virtual Value process(const FunctionCall& exp) = 0;
};


class Node {
public:

	virtual Value evaluate(TreeVisitor& visitor) = 0;

};

class Expression : public Node {

public:

	virtual Value evaluate(TreeVisitor& visitor) override;

	using Ptr = std::shared_ptr<Expression>;

};

class Unary final : public Expression {
public:

	Unary(Operator _op, const Expression::Ptr& _exp) : op(_op), exp(_exp) {}

	Value evaluate(TreeVisitor& visitor) override;

	const Operator op;
	const Expression::Ptr exp;
};

class Binary final : public Expression {
public:

	Binary(Operator _op, const Expression::Ptr& _left, const Expression::Ptr& _right) : op(_op), left(_left), right(_right) {}

	Value evaluate(TreeVisitor& visitor) override;

	const Operator op;
	const Expression::Ptr left;
	const Expression::Ptr right;
};

class Ternary final : public Expression {
public:

	Ternary(const Expression::Ptr& _condition, const Expression::Ptr& _pass, const Expression::Ptr& _fail) : condition(_condition), pass(_pass), fail(_fail) {}

	Value evaluate(TreeVisitor& visitor) override;

	const Expression::Ptr condition;
	const Expression::Ptr pass;
	const Expression::Ptr fail;
};

class Member final : public Expression {
public:

	Member(const Expression::Ptr& _parent, const std::string& _member) : parent(_parent), member(_member) {}

	Value evaluate(TreeVisitor& visitor) override;

	const Expression::Ptr parent;
	const std::string member;
};

class Literal final : public Expression {
public:

	Literal(double _val) : val(_val){}

	Value evaluate(TreeVisitor& visitor) override;

	const double val;
};

class Variable final : public Expression {
public:

	Variable(const std::string& _name) : name(_name){}

	Value evaluate(TreeVisitor& visitor) override;

	const std::string name;
};

class FunctionCall final : public Expression {
public:

	FunctionCall(const std::string& _name, const std::vector<Expression::Ptr>& _members) : name(_name), members(_members) {}

	Value evaluate(TreeVisitor& visitor) override;

	const std::string name;
	const std::vector<Expression::Ptr> members;

};
