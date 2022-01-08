#pragma once
#include "core/Common.hpp"
#include <unordered_map>

enum Format : uint {
	// Use 3 bits for flags
	// | row(0) / column(1) | bin(00) / dec(11) / oct(01) / hex(10) |
	// INTERNAL = mark 1 extra bit (4 bits total)
	MAJOR_MASK = 0b001,
	MAJOR_ROW_FLAG = 0b000,
	MAJOR_COL_FLAG = 0b001, // so that INTERNAL & MAJOR_MASK is column major

	BASE_MASK = 0b110,
	BASE_2_FLAG = 0b000,
	BASE_8_FLAG = 0b010,
	BASE_16_FLAG = 0b100,
	BASE_10_FLAG = 0b110, // so that INTERNAL & BASE_MASK is base 10

	INTERNAL = 0b1111
};

enum class Operator {
	OpenParenth, CloseParenth, Plus, Minus, Product, Divide, Power, Modulo, Assign, ShiftLeft, ShiftRight, BitOr, BitAnd, BitNot, BitXor, BoolOr, BoolAnd, BoolNot, BoolXor, QuestionMark, Colon, LessThan, GreaterThan, LessThanEqual, GreaterThanEqual, Equal, Different, Comma, Dot
};

static const std::unordered_map<std::string, double> MathConstants = {
	{"pi", glm::pi<double>() },
	{"deg",  glm::pi<double>() / 180.0 },
};

inline std::string OperatorString(Operator op){
	static const std::vector<std::string> opStrs = {
		"(", ")", "+", "-", "*", "/", "^", "%", "=", "<<", ">>", "|", "&", "~", "#", "||", "&&", "!", "##", "?", ":", "<", ">", "<=", ">=", "==", "!=", ",", "."
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
		VEC3,
		VEC4,
		MAT3,
		MAT4,
		STRING
	};

	Value() : type(STRING), str("empty"){}

	Value(bool val) : type(BOOL), b(val){}

	Value(long long val) : type(INTEGER), i(val){}

	Value(double val) : type(FLOAT), f(val){}

	Value(const std::string& val) : type(STRING), str(val){}
	
	Value(const glm::vec3& val) : type(VEC3), v3(val){}

	Value(const glm::vec4& val) : type(VEC4), v4(val){}

	Value(const glm::mat3& val) : type(MAT3), m3(val){}

	Value(const glm::mat4& val) : type(MAT4), m4(val){}

	bool convert(const Type& target, Value& outVal) const;

	std::string toString(Format format) const;

	Type type;
	union {
		glm::mat4 m4;
		glm::mat3 m3;
		glm::vec4 v4;
		glm::vec3 v3;
		double f;
		long long i;
		bool b;
	};
	std::string str;
};

inline std::string TypeString(Value::Type type){
	static const std::vector<std::string> typeStrs = {
		"boolean", "integer", "float", "vec3", "vec4", "mat3", "mat4", "string"
	};
	return typeStrs[type];
}

class Unary;
class Binary;
class Ternary;
class Member;
class Literal;
class Variable;
class VariableDef;
class FunctionDef;
class FunctionVar;
class FunctionCall;
class Expression;

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
	virtual Value process(		FunctionVar& exp) = 0; // This is intentional, only used when flattening function definitions.
	virtual Value process(const FunctionCall& exp) = 0;

	Status getStatus() const;

	Expression const * getErrorExpression() const {
		return _failedExpression;
	}

	void registerError(const std::string& msg, Expression const * exp);

protected:

	std::string _failedMessage;
	Expression const * _failedExpression = nullptr;
	bool _failed = false;
};

class Expression {

public:

	Expression(long _start, long _end) : dbgStartPos(_start), dbgEndPos(_end) {}

	virtual Value evaluate(TreeVisitor& visitor) = 0;

	Status evaluate(TreeVisitor& visitor, Value& outValue);

	using Ptr = std::shared_ptr<Expression>;

	const long dbgStartPos;
	const long dbgEndPos;
	
};

class Unary final : public Expression {
public:

	Unary(Operator _op, const Expression::Ptr& _exp, long _start, long _end)
		: Expression(_start, _end), op(_op), exp(_exp) {}

	Value evaluate(TreeVisitor& visitor) override;

	const Operator op;
	const Expression::Ptr exp;
};

class Binary final : public Expression {
public:

	Binary(Operator _op, const Expression::Ptr& _left, const Expression::Ptr& _right, long _start, long _end)
		: Expression(_start, _end), op(_op), left(_left), right(_right) {}

	Value evaluate(TreeVisitor& visitor) override;

	const Operator op;
	const Expression::Ptr left;
	const Expression::Ptr right;
};

class Ternary final : public Expression {
public:

	Ternary(const Expression::Ptr& _condition, const Expression::Ptr& _pass, const Expression::Ptr& _fail, long _start, long _end)
		: Expression(_start, _end), condition(_condition), pass(_pass), fail(_fail) {}

	Value evaluate(TreeVisitor& visitor) override;

	const Expression::Ptr condition;
	const Expression::Ptr pass;
	const Expression::Ptr fail;
};

class Member final : public Expression {
public:

	Member(const Expression::Ptr& _parent, const std::string& _member, long _start)
		: Expression(_start, _start), parent(_parent), member(_member) {}

	Value evaluate(TreeVisitor& visitor) override;

	const Expression::Ptr parent;
	const std::string member;
};

class Literal final : public Expression {
public:

	Literal(const Value& _val, long _start)
		: Expression(_start, _start), val(_val){}

	Value evaluate(TreeVisitor& visitor) override;

	const Value val;
};

class Variable final : public Expression {
public:

	Variable(const std::string& _name, long _start)
		: Expression(_start, _start), name(_name){}

	Value evaluate(TreeVisitor& visitor) override;

	const std::string name;

};

class VariableDef final : public Expression {
public:

	VariableDef(const std::string& _name, const Expression::Ptr& _expr, long _start)
		: Expression(_start, _start), name(_name), expr(_expr) {}

	Value evaluate(TreeVisitor& visitor) override;

	const std::string name;
	const Expression::Ptr expr;
};

class FunctionDef final : public Expression {
public:

	FunctionDef(const std::string& _name, const std::vector<std::string>& _args, const Expression::Ptr& _expr, long _start)
		: Expression(_start, _start), name(_name), args(_args), expr(_expr) {}

	Value evaluate(TreeVisitor& visitor) override;

	const std::string name;
	std::vector<std::string> args;
	const Expression::Ptr expr;

};

class FunctionVar final : public Expression {
public:

	FunctionVar(const std::string& _name, long _start)
		: Expression(_start, _start), name(_name){}

	Value evaluate(TreeVisitor& visitor) override;

	void setValue(const Value& value){
		_hasValue = true;
		_value = value;
	}

	bool hasValue() const {
		return _hasValue;
	}

	const Value& value() const {
		assert(hasValue());
		return _value;
	}

	std::string name;

private:

	Value _value;
	bool _hasValue = false;
};

class FunctionCall final : public Expression {
public:

	FunctionCall(const std::string& _name, const std::vector<Expression::Ptr>& _args, long _start, long _end)
		: Expression(_start, _end), name(_name), args(_args) {}

	Value evaluate(TreeVisitor& visitor) override;

	const std::string name;
	const std::vector<Expression::Ptr> args;

};
