#pragma once
#include "core/Common.hpp"
#include "core/Types.hpp"

class TreeLogger final : public TreeVisitor {
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

class Evaluator {
public:
	Evaluator(const Expression::Ptr& tree) : _tree(tree) {

	}

	std::string log();


private:
	Expression::Ptr _tree;

};
