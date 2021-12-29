#pragma once
#include "core/Common.hpp"
#include "core/Types.hpp"

class Evaluator {
public:
	Evaluator(const Expression::Ptr& tree) : _tree(tree) {}

	std::string log();

	Value eval();

	void registerError(const Expression::Ptr& exp, const std::string& msg);

private:
	Expression::Ptr _tree;

	std::string _failedMessage;
	Expression::Ptr _failedExpression = nullptr;
	bool _failed = false;
};


class ExpLogger final : public TreeVisitor {
public:
	Value process(const Unary& exp) override;
	Value process(const Binary& exp) override;
	Value process(const Ternary& exp) override;
	Value process(const Member& exp) override;
	Value process(const Literal& exp) override;
	Value process(const Variable& exp) override;
	Value process(const VariableDef& exp) override;
	Value process(const FunctionDef& exp) override;
	Value process(const FunctionCall& exp) override;
};

class ExpEval final : public TreeVisitor {
public:
	ExpEval(Evaluator& context);

	Value process(const Unary& exp) override;
	Value process(const Binary& exp) override;
	Value process(const Ternary& exp) override;
	Value process(const Member& exp) override;
	Value process(const Literal& exp) override;
	Value process(const Variable& exp) override;
	Value process(const VariableDef& exp) override;
	Value process(const FunctionDef& exp) override;
	Value process(const FunctionCall& exp) override;

private:
	Evaluator& _context;
};
