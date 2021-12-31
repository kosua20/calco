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
	ExpEval(const Scope& scope, FunctionsLibrary& stdlib);

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
	bool convertValues(const Value& l, const Value& r, Value::Type type, Value& outl, Value& outr);
	bool alignValues(const Value& l, const Value& r, Value& outl, Value& outr, Value::Type minType);

	Value uOpIdentity(const Value& v);
	Value uOpNegate(const Value& v);
	Value uOpBitNot(const Value& v);
	Value uOpBoolNot(const Value& v);
	Value bOpAddition(const Value& l, const Value& r);
	Value bOpSubstraction(const Value& l, const Value& r);
	Value bOpProduct(const Value& l, const Value& r);
	Value bOpDivide(const Value& l, const Value& r);
	Value bOpPower(const Value& l, const Value& r);
	Value bOpModulo(const Value& l, const Value& r);
	Value bOpShiftLeft(const Value& l, const Value& r);
	Value bOpShiftRight(const Value& l, const Value& r);
	Value bOpLessThan(const Value& l, const Value& r);
	Value bOpGreaterThan(const Value& l, const Value& r);
	Value bOpLessThanEqual(const Value& l, const Value& r);
	Value bOpGreaterThanEqual(const Value& l, const Value& r);
	Value bOpEqual(const Value& l, const Value& r);
	Value bOpNotEqual(const Value& l, const Value& r);
	Value bOpBitOr(const Value& l, const Value& r);
	Value bOpBitAnd(const Value& l, const Value& r);
	Value bOpBitXor(const Value& l, const Value& r);
	Value bOpBoolOr(const Value& l, const Value& r);
	Value bOpBoolAnd(const Value& l, const Value& r);
	Value bOpBoolXor(const Value& l, const Value& r);

	const Scope& _globalScope;
	FunctionsLibrary& _stdlib;

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
