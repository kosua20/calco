#pragma once
#include "core/Common.hpp"
#include "core/Types.hpp"
#include "core/Functions.hpp"

class Evaluator {
public:
	Evaluator(const Expression::Ptr& tree, Scope& scope) : _tree(tree), _globalScope(scope) {}

	std::string log();

	Value eval();

	Status substitute(std::vector<std::string>& argNames, const std::string& id);

	void registerError(const Expression::Ptr& exp, const std::string& msg);

	Scope& globalScope() { return _globalScope; }
	
private:
	Expression::Ptr _tree;
	Scope& _globalScope;

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
	Value process( 		FunctionVar& exp) override;
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
	Value process(		FunctionVar& exp) override;
	Value process(const FunctionCall& exp) override;

private:
	Evaluator& _context;
	std::stack<Scope> _localScopes;
};

class FuncSubstitution final : public TreeVisitor {
public:
	FuncSubstitution(Evaluator& context, const std::vector<std::string>& argNames, const std::string& id);

	Value process(const Unary& exp) override;
	Value process(const Binary& exp) override;
	Value process(const Ternary& exp) override;
	Value process(const Member& exp) override;
	Value process(const Literal& exp) override;
	Value process(const Variable& exp) override;
	Value process(const VariableDef& exp) override;
	Value process(const FunctionDef& exp) override;
	Value process(		FunctionVar& exp) override;
	Value process(const FunctionCall& exp) override;

private:
	Evaluator& _context;
	const std::vector<std::string>& _names;
	const std::string _id;
};
