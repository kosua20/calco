#pragma once
#include "core/Common.hpp"
#include "core/Types.hpp"

class TreeLogger final : public TreeVisitor {
public:
	Value process(const Expression& exp) override;
	Value process(const Unary& exp) override;
	Value process(const Binary& exp) override;
	Value process(const Ternary& exp) override;
	Value process(const Member& exp) override;
	Value process(const Literal& exp) override;
	Value process(const Variable& exp) override;
	Value process(const FunctionCall& exp) override;
};

class Evaluator {
public:
	Evaluator(const std::shared_ptr<Node>& tree) : _tree(tree) {

	}

	std::string log();


private:
	std::shared_ptr<Node> _tree;

};
