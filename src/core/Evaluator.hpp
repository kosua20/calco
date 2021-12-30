#pragma once
#include "core/Common.hpp"
#include "core/Types.hpp"
#include "core/Functions.hpp"

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
	ExpEval(const Scope& scope, const FunctionsLibrary& stdlib);

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
	const Scope& _globalScope;
	const FunctionsLibrary& _stdlib;

	std::stack<Scope> _localScopes;

};

class FuncSubstitution final : public TreeVisitor {
public:
	FuncSubstitution(const Scope& _scope, const FunctionsLibrary& stdlib, const std::vector<std::string>& argNames, const std::string& id);

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
	const Scope& _globalScope;
	const FunctionsLibrary& _stdlib;

	const std::vector<std::string>& _names;
	const std::string _id;

};
