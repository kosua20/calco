#pragma once
#include "core/Common.hpp"
#include <unordered_map>

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


struct Status {

	std::string message;
	long location = -1;
	bool success = true;

	explicit Status(bool _success = true) : success(_success) {};

	Status(long _location, const std::string& _message) : message(_message), location(_location), success(false) {};

	operator bool() const {
		return success;
	}
};

struct Value {

	enum Type {
		BOOL = 0,
		INTEGER,
		FLOAT,
		VEC4,
		MAT4,
		STRING
	};

	Value() : type(STRING), str("empty"){}

	Value(bool val) : type(BOOL), b(val){}

	Value(long long val) : type(INTEGER), i(val){}

	Value(double val) : type(FLOAT), f(val){}

	Value(const std::string& val) : type(STRING), str(val){}

	Value(const glm::mat4& val) : type(MAT4), mat(val){}
	
	Value(const glm::vec4& val) : type(VEC4), vec(val){}

	Value convert(const Type& target, bool& success) const;

	Type type;
	glm::mat4 mat;
	glm::vec4 vec;
	double f;
	long long i;
	bool b;
	std::string str;
};


class Unary;
class Binary;
class Ternary;
class Member;
class Literal;
class Variable;
class VariableDef;
class FunctionDef;
class FunctionCall;

class TreeVisitor {
public:
	virtual Value process(const Unary& exp) = 0;
	virtual Value process(const Binary& exp) = 0;
	virtual Value process(const Ternary& exp) = 0;
	virtual Value process(const Member& exp) = 0;
	virtual Value process(const Literal& exp) = 0;
	virtual Value process(const Variable& exp) = 0;
	virtual Value process(const VariableDef& exp) = 0;
	virtual Value process(const FunctionDef& exp) = 0;
	virtual Value process(const FunctionCall& exp) = 0;
};

class Expression {

public:

	virtual Value evaluate(TreeVisitor& visitor) = 0;

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

	Literal(const Value& _val) : val(_val){}

	Value evaluate(TreeVisitor& visitor) override;

	const Value val;
};

class Variable final : public Expression {
public:

	Variable(const std::string& _name) : name(_name){}

	Value evaluate(TreeVisitor& visitor) override;

	const std::string name;
};

class VariableDef final : public Expression {
public:

	VariableDef(const std::string& _name, const Expression::Ptr& _expr) : name(_name), expr(_expr) {}

	Value evaluate(TreeVisitor& visitor) override;

	const std::string name;
	const Expression::Ptr expr;
};

class FunctionDef final : public Expression {
public:

	FunctionDef(const std::string& _name, const std::vector<std::shared_ptr<Variable>>& _args, const Expression::Ptr& _expr) : name(_name), args(_args), expr(_expr) {}

	Value evaluate(TreeVisitor& visitor) override;

	const std::string name;
	const std::vector<std::shared_ptr<Variable>> args;
	const Expression::Ptr expr;

};

class FunctionCall final : public Expression {
public:

	FunctionCall(const std::string& _name, const std::vector<Expression::Ptr>& _args) : name(_name), args(_args) {}

	Value evaluate(TreeVisitor& visitor) override;

	const std::string name;
	const std::vector<Expression::Ptr> args;

};
