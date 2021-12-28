#include "core/Evaluator.hpp"

Value TreeLogger::process(const Expression& exp) {
	(void)exp;
	assert(false);
	return std::string("Empty expression");
}

Value TreeLogger::process(const Unary& exp)  {
	const std::string str = exp.exp->evaluate(*this).str;
	return "( " + OperatorString(exp.op) + str + " )";
}

Value TreeLogger::process(const Binary& exp)  {
	const std::string strL = exp.left->evaluate(*this).str;
	const std::string strR = exp.right->evaluate(*this).str;
	return "( " + strL + " " + OperatorString(exp.op) + " " + strR + " )";
}

Value TreeLogger::process(const Ternary& exp) {
	const std::string strC = exp.condition->evaluate(*this).str;
	const std::string strP = exp.pass->evaluate(*this).str;
	const std::string strF = exp.fail->evaluate(*this).str;
	return "( " + strC + " ? " + strP + " : " + strF + " )";
}

Value TreeLogger::process(const Member& exp) {
	const std::string str = exp.parent->evaluate(*this).str;
	return "( " + str + "." + exp.member + " )";
}

Value TreeLogger::process(const Literal& exp) {
	return std::to_string(exp.val);
}

Value TreeLogger::process(const Variable& exp) {
	return exp.name;
}

Value TreeLogger::process(const FunctionCall& exp)  {
	std::string args;
	const size_t argCount = exp.members.size();
	for(size_t aid = 0; aid < argCount; ++aid){
		const auto& arg = exp.members[aid];
		const std::string str = arg->evaluate(*this).str;
		args += str + ", ";
	}
	return exp.name + "( " + args + " )";
}

std::string Evaluator::log(){

	TreeLogger logger;
	Value finalStr = _tree->evaluate(logger);
	return finalStr.str;
}
