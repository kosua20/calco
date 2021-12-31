#include "core/Evaluator.hpp"

Value ExpLogger::process(const Unary& exp)  {
	const std::string str = exp.exp->evaluate(*this).str;
	return "( " + OperatorString(exp.op) + str + " )";
}

Value ExpLogger::process(const Binary& exp)  {
	const std::string strL = exp.left->evaluate(*this).str;
	const std::string strR = exp.right->evaluate(*this).str;
	return "( " + strL + " " + OperatorString(exp.op) + " " + strR + " )";
}

Value ExpLogger::process(const Ternary& exp) {
	const std::string strC = exp.condition->evaluate(*this).str;
	const std::string strP = exp.pass->evaluate(*this).str;
	const std::string strF = exp.fail->evaluate(*this).str;
	return "( " + strC + " ? " + strP + " : " + strF + " )";
}

Value ExpLogger::process(const Member& exp) {
	const std::string str = exp.parent->evaluate(*this).str;
	return "( " + str + "." + exp.member + " )";
}

Value ExpLogger::process(const Literal& exp) {
	switch(exp.val.type){
		case Value::INTEGER:
			return std::to_string(exp.val.i);
		case Value::FLOAT:
			return std::to_string(exp.val.f);
		default:
			break;
	}
	return std::string("(complex type value)");
}

Value ExpLogger::process(const Variable& exp) {
	return exp.name;
}

Value ExpLogger::process(const VariableDef& exp) {
	return exp.name + " = " + exp.expr->evaluate(*this).str;
}

Value ExpLogger::process(const FunctionDef& exp)  {
	std::string args;
	const size_t argCount = exp.args.size();
	for(size_t aid = 0; aid < argCount; ++aid){
		const auto& arg = exp.args[aid];
		args += (aid != 0 ? "," : "") + arg;
	}
	return exp.name + "( " + args + " ) = " + exp.expr->evaluate(*this).str;
}

Value ExpLogger::process(FunctionVar& exp) {
	return exp.name;
}

Value ExpLogger::process(const FunctionCall& exp)  {
	std::string args;
	const size_t argCount = exp.args.size();
	for(size_t aid = 0; aid < argCount; ++aid){
		const auto& arg = exp.args[aid];
		const std::string str = arg->evaluate(*this).str;
		args += (aid != 0 ? "," : "") + str;
	}
	return exp.name + "( " + args + " )";
}

#define EXIT(exp, msg) if(true){ if(!_failed){ _failedMessage = msg; _failedExpression = exp; }; _failed = true; return false; }

ExpEval::ExpEval(const Scope& scope, FunctionsLibrary& stdlib) : _globalScope(scope), _stdlib(stdlib) {}

Value ExpEval::uOpIdentity(const Value& v){
	switch(v.type){
		case Value::INTEGER:
			return v.i;
		case Value::FLOAT:
			return v.f;
		case Value::VEC3:
			return v.v3;
		case Value::MAT3:
			return v.m3;
		case Value::VEC4:
			return v.v4;
		case Value::MAT4:
			return v.m4;
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(v.type) + " for unary + sign.");
}

Value ExpEval::uOpNegate(const Value& v){
	switch(v.type){
		case Value::INTEGER:
			return -v.i;
		case Value::FLOAT:
			return -v.f;
		case Value::VEC3:
			return -v.v3;
		case Value::MAT3:
			return -v.m3;
		case Value::VEC4:
			return -v.v4;
		case Value::MAT4:
			return -v.m4;
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(v.type) + " for unary - sign.");
}

Value ExpEval::uOpBitNot(const Value& v){
	Value vc;
	if(v.convert(Value::INTEGER, vc)){
		return ~v.i;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(v.type) + " for bitwise negation.");
}

Value ExpEval::uOpBoolNot(const Value& v){
	Value vc;
	if(v.convert(Value::BOOL, vc)){
		return !v.b;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(v.type) + " for boolean negation.");
}

Value ExpEval::process(const Unary& exp)  {
	Value v = exp.exp->evaluate(*this);
	// Early exit.
	if(_failed){
		return false;
	}

	static const std::unordered_map<Operator, Value (ExpEval::*)(const Value& v)> unaryOps = {
		{ Operator::Plus, &ExpEval::uOpIdentity }, { Operator::Minus, &ExpEval::uOpNegate },
		{ Operator::BitNot, &ExpEval::uOpBitNot }, { Operator::BoolNot, &ExpEval::uOpBoolNot }
	};
	auto uOp = unaryOps.find(exp.op);
	if(uOp != unaryOps.end()){
		return (this->*(uOp->second))(v);
	}
	EXIT(&exp, "Unknown unary operator: " + OperatorString(exp.op));
}

bool ExpEval::convertValues(const Value& l, const Value& r, Value::Type type, Value& outl, Value& outr){
	if(!l.convert(type, outl)){
		EXIT(nullptr, "Unable to convert left member to " + TypeString(type) + ".");
	}
	if(!r.convert(type, outr)){
		EXIT(nullptr, "Unable to convert right member to " + TypeString(type)  + ".");
	}
	return true;
}

bool ExpEval::alignValues(const Value& l, const Value& r, Value& outl, Value& outr, Value::Type minType){
	const Value::Type maxType = std::max(minType, std::max(l.type, r.type));
	return convertValues(l, r, maxType, outl, outr);
}

Value ExpEval::bOpAddition(const Value& l, const Value& r){
	Value outl, outr;
	if(!alignValues(l, r, outl, outr, Value::INTEGER)){
		return false;
	}
	switch(outl.type){
		case Value::INTEGER:
			return outl.i + outr.i;
		case Value::FLOAT:
			return outl.f + outr.f;
		case Value::VEC3:
			return outl.v3 + outr.v3;
		case Value::MAT3:
			return outl.m3 + outr.m3;
		case Value::VEC4:
			return outl.v4 + outr.v4;
		case Value::MAT4:
			return outl.m4 + outr.m4;
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(outl.type) + " for addition.");
	return false;
}

Value ExpEval::bOpSubstraction(const Value& l, const Value& r){
	Value outl, outr;
	if(!alignValues(l, r, outl, outr, Value::INTEGER)){
		return false;
	}
	switch(outl.type){
		case Value::INTEGER:
			return outl.i - outr.i;
		case Value::FLOAT:
			return outl.f - outr.f;
		case Value::VEC3:
			return outl.v3 - outr.v3;
		case Value::MAT3:
			return outl.m3 - outr.m3;
		case Value::VEC4:
			return outl.v4 - outr.v4;
		case Value::MAT4:
			return outl.m4 - outr.m4;
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(outl.type) + " for substraction.");
	return false;
}

Value ExpEval::bOpProduct(const Value& l, const Value& r){
	// Special case: matrix * vec or vec * matrix.
	if(l.type == Value::MAT3 && r.type == Value::VEC3){
		return l.m3 * r.v3;
	}
	if(l.type == Value::VEC3 && r.type == Value::MAT3){
		return l.v3 * r.m3;
	}
	if(l.type == Value::MAT4 && r.type == Value::VEC4){
		return l.m4 * r.v4;
	}
	if(l.type == Value::VEC4 && r.type == Value::MAT4){
		return l.v4 * r.m4;
	}

	// All other cases are covered via type promotion.
	Value outl, outr;
	if(!alignValues(l, r, outl, outr, Value::INTEGER)){
		return false;
	}
	switch(outl.type){
		case Value::INTEGER:
			return outl.i * outr.i;
		case Value::FLOAT:
			return outl.f * outr.f;
		case Value::VEC3:
			return outl.v3 * outr.v3;
		case Value::MAT3:
			return outl.m3 * outr.m3;
		case Value::VEC4:
			return outl.v4 * outr.v4;
		case Value::MAT4:
			return outl.m4 * outr.m4;
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(outl.type) + " for product.");
	return false;
}

Value ExpEval::bOpDivide(const Value& l, const Value& r){
	Value outl, outr;
	if(!alignValues(l, r, outl, outr, Value::FLOAT)){
		return false;
	}
	switch(outl.type){
		case Value::FLOAT:
			return outl.f / outr.f;
		case Value::VEC3:
			return outl.v3 / outr.v3;
		case Value::MAT3:
			return outl.m3 / outr.m3;
		case Value::VEC4:
			return outl.v4 / outr.v4;
		case Value::MAT4:
			return outl.m4 / outr.m4;
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(outl.type) + " for division.");
}

Value ExpEval::bOpPower(const Value& l, const Value& r){
	Value outl, outr;
	if(!alignValues(l, r, outl, outr, Value::FLOAT)){
		return false;
	}
	switch(outl.type){
		case Value::FLOAT:
			return glm::pow(outl.f, outr.f);
		case Value::VEC3:
			return glm::pow(outl.v3, outr.v3);
		case Value::VEC4:
			return glm::pow(outl.v4, outr.v4);
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(outl.type) + " for exponentiation.");
}

Value ExpEval::bOpModulo(const Value& l, const Value& r){
	Value outl, outr;
	if(!alignValues(l, r, outl, outr, Value::INTEGER)){
		return false;
	}
	switch(outl.type){
		case Value::INTEGER:
			return outl.i % outr.i;
		case Value::FLOAT:
			return glm::mod(outl.f, outr.f);
		case Value::VEC3:
			return glm::mod(outl.v3, outr.v3);
		case Value::VEC4:
			return glm::mod(outl.v4, outr.v4);
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(outl.type) + " for modulo.");
}

Value ExpEval::bOpShiftLeft(const Value& l, const Value& r){
	Value outl, outr;
	if(!convertValues(l, r, Value::INTEGER, outl, outr)){
		return false;
	}
	return outl.i << outr.i;
}

Value ExpEval::bOpShiftRight(const Value& l, const Value& r){
	Value outl, outr;
	if(!convertValues(l, r, Value::INTEGER, outl, outr)){
		return false;
	}
	return outl.i >> outr.i;
}

Value ExpEval::bOpLessThan(const Value& l, const Value& r){
	Value outl, outr;
	if(!alignValues(l, r, outl, outr, Value::INTEGER)){
		return false;
	}
	switch(outl.type){
		case Value::INTEGER:
			return outl.i < outr.i;
		case Value::FLOAT:
			return outl.f < outr.f;
		case Value::VEC3:
			return glm::all(glm::lessThan(outl.v3, outr.v3));
		case Value::VEC4:
			return glm::all(glm::lessThan(outl.v4, outr.v4));
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(outl.type) + " for comparison.");
}

Value ExpEval::bOpGreaterThan(const Value& l, const Value& r){
	Value outl, outr;
	if(!alignValues(l, r, outl, outr, Value::INTEGER)){
		return false;
	}
	switch(outl.type){
		case Value::INTEGER:
			return outl.i > outr.i;
		case Value::FLOAT:
			return outl.f > outr.f;
		case Value::VEC3:
			return glm::all(glm::greaterThan(outl.v3, outr.v3));
		case Value::VEC4:
			return glm::all(glm::greaterThan(outl.v4, outr.v4));
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(outl.type) + " for comparison.");
}

Value ExpEval::bOpLessThanEqual(const Value& l, const Value& r){
	Value outl, outr;
	if(!alignValues(l, r, outl, outr, Value::INTEGER)){
		return false;
	}
	switch(outl.type){
		case Value::INTEGER:
			return outl.i <= outr.i;
		case Value::FLOAT:
			return outl.f <= outr.f;
		case Value::VEC3:
			return glm::all(glm::lessThanEqual(outl.v3, outr.v3));
		case Value::VEC4:
			return glm::all(glm::lessThanEqual(outl.v4, outr.v4));
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(outl.type) + " for comparison.");
}

Value ExpEval::bOpGreaterThanEqual(const Value& l, const Value& r){
	Value outl, outr;
	if(!alignValues(l, r, outl, outr, Value::INTEGER)){
		return false;
	}
	switch(outl.type){
		case Value::INTEGER:
			return outl.i >= outr.i;
		case Value::FLOAT:
			return outl.f >= outr.f;
		case Value::VEC3:
			return glm::all(glm::greaterThanEqual(outl.v3, outr.v3));
		case Value::VEC4:
			return glm::all(glm::greaterThanEqual(outl.v4, outr.v4));
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(outl.type) + " for comparison.");
}

Value ExpEval::bOpEqual(const Value& l, const Value& r){
	Value outl, outr;
	if(!alignValues(l, r, outl, outr, Value::BOOL)){
		return false;
	}
	switch(outl.type){
		case Value::BOOL:
			return outl.b == outr.b;
		case Value::INTEGER:
			return outl.i == outr.i;
		case Value::FLOAT:
			return outl.f == outr.f;
		case Value::VEC3:
			return outl.v3 == outr.v3;
		case Value::MAT3:
			return outl.m3 == outr.m3;
		case Value::VEC4:
			return outl.v4 == outr.v4;
		case Value::MAT4:
			return outl.m4 == outr.m4;
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(outl.type) + " for comparison.");
}

Value ExpEval::bOpNotEqual(const Value& l, const Value& r){
	Value outl, outr;
	if(!alignValues(l, r, outl, outr, Value::BOOL)){
		return false;
	}
	switch(outl.type){
		case Value::BOOL:
			return outl.b != outr.b;
		case Value::INTEGER:
			return outl.i != outr.i;
		case Value::FLOAT:
			return outl.f != outr.f;
		case Value::VEC3:
			return outl.v3 != outr.v3;
		case Value::MAT3:
			return outl.m3 != outr.m3;
		case Value::VEC4:
			return outl.v4 != outr.v4;
		case Value::MAT4:
			return outl.m4 != outr.m4;
		default:
			break;
	}
	EXIT(nullptr, "Unsupported type " + TypeString(outl.type) + " for comparison.");
}

Value ExpEval::bOpBitOr(const Value& l, const Value& r){
	Value outl, outr;
	if(!convertValues(l, r, Value::INTEGER, outl, outr)){
		return false;
	}
	return outl.i | outr.i;
}

Value ExpEval::bOpBitAnd(const Value& l, const Value& r){
	Value outl, outr;
	if(!convertValues(l, r, Value::INTEGER, outl, outr)){
		return false;
	}
	return outl.i & outr.i;
}

Value ExpEval::bOpBitXor(const Value& l, const Value& r){
	Value outl, outr;
	if(!convertValues(l, r, Value::INTEGER, outl, outr)){
		return false;
	}
	return outl.i ^ outr.i;
}

Value ExpEval::bOpBoolOr(const Value& l, const Value& r){
	Value outl, outr;
	if(!convertValues(l, r, Value::BOOL, outl, outr)){
		return false;
	}
	return outl.b || outr.b;
}

Value ExpEval::bOpBoolAnd(const Value& l, const Value& r){
	Value outl, outr;
	if(!convertValues(l, r, Value::BOOL, outl, outr)){
		return false;
	}
	return outl.b || outr.b;
}

Value ExpEval::bOpBoolXor(const Value& l, const Value& r){
	Value outl, outr;
	if(!convertValues(l, r, Value::BOOL, outl, outr)){
		return false;
	}
	return (!outl.b && outr.b) || (outl.b && !outr.b);
}


Value ExpEval::process(const Binary& exp)  {

	// No notion of partial evaluation.
	const Value l = exp.left->evaluate(*this);
	const Value r = exp.right->evaluate(*this);
	// Early exit.
	if(_failed){
		return false;
	}

	static const std::unordered_map<Operator, Value (ExpEval::*)(const Value& l, const Value& r)> binaryOps = {
		{ Operator::Plus, &ExpEval::bOpAddition }, { Operator::Minus, &ExpEval::bOpSubstraction },
		{ Operator::Product, &ExpEval::bOpProduct }, { Operator::Divide, &ExpEval::bOpDivide },
		{ Operator::Power, &ExpEval::bOpPower }, { Operator::Modulo, &ExpEval::bOpModulo },
		{ Operator::ShiftLeft, &ExpEval::bOpShiftLeft }, { Operator::ShiftRight, &ExpEval::bOpShiftRight },
		{ Operator::LessThan, &ExpEval::bOpLessThan }, { Operator::GreaterThan, &ExpEval::bOpGreaterThan },
		{ Operator::LessThanEqual, &ExpEval::bOpLessThanEqual },{ Operator::GreaterThanEqual, &ExpEval::bOpGreaterThanEqual },
		{ Operator::Equal, &ExpEval::bOpEqual }, { Operator::Different, &ExpEval::bOpNotEqual },
		{ Operator::BitOr, &ExpEval::bOpBitOr }, { Operator::BitAnd, &ExpEval::bOpBitAnd }, { Operator::BitXor, &ExpEval::bOpBitXor },
		{ Operator::BoolOr, &ExpEval::bOpBoolOr }, { Operator::BoolAnd, &ExpEval::bOpBoolAnd }, { Operator::BoolXor, &ExpEval::bOpBoolXor },
	};
	auto bOp = binaryOps.find(exp.op);
	if(bOp != binaryOps.end()){
		return (this->*(bOp->second))(l, r);
	}

	EXIT(&exp, "Unknown binary operator: " + OperatorString(exp.op));
}

Value ExpEval::process(const Ternary& exp) {
	const Value cond = exp.condition->evaluate(*this);

	// Cast to bool.
	Value condBool;
	if(!cond.convert(Value::BOOL, condBool)){
		EXIT(&exp, "Condition could not be converted to a boolean.");
	}

	// Partial evaluation.
	if(condBool.b){
		return exp.pass->evaluate(*this);
	}
	return exp.fail->evaluate(*this);
}

Value ExpEval::process(const Member& exp) {
	Value par = exp.parent->evaluate(*this);

	// Only on vector types.
	if(par.type != Value::VEC3 && par.type != Value::MAT3 && par.type != Value::VEC4 && par.type != Value::MAT4){
		EXIT(&exp, "Subscripts are only supported on vector/matrix types.");
	}

	// Check that the subscript can be converted to a set of indices.
	static const std::unordered_map<char, size_t> getters = {
		{'x', 0}, {'y', 1}, {'z', 2}, {'w', 3}
	};

	const size_t getSize = exp.member.size();
	std::vector<size_t> indices(getSize);
	for(size_t cid = 0; cid < getSize; ++cid){
		const char& c = exp.member.at(cid);
		if(getters.count(c) == 0){
			EXIT(&exp, "Unknown subscript " + exp.member + ".");
		}
		indices[cid] = getters.at(c);
	}

	// Size check (after the conversion, so that 'v.thing' triggers a better error message)
	if(getSize > 4){
		EXIT(&exp, "Subscript " + exp.member + " is too long.");
	}

	// Prevalidation for smaller types.
	if(par.type == Value::VEC3 || par.type == Value::MAT3){
		for(size_t cid = 0; cid < getSize; ++cid){
			if(indices[cid] >= 3){
				EXIT(&exp, "Subscript " + exp.member + " is out of bound for type " + TypeString(par.type) + ".");
			}
		}
	}

	switch (par.type) {
		case Value::VEC3:
			if(getSize == 1){
				return par.v3[indices[0]];
			}
			if(getSize == 3){
				return glm::vec3(par.v3[indices[0]], par.v3[indices[1]], par.v3[indices[2]]);
			}
			if(getSize == 4){
				return glm::vec4(par.v3[indices[0]], par.v3[indices[1]], par.v3[indices[2]], par.v3[indices[3]]);
			}
			break;
		case Value::VEC4:
			if(getSize == 1){
				return par.v4[indices[0]];
			}
			if(getSize == 3){
				return glm::vec3(par.v4[indices[0]], par.v4[indices[1]], par.v4[indices[2]]);
			}
			if(getSize == 4){
				return glm::vec4(par.v4[indices[0]], par.v4[indices[1]], par.v4[indices[2]], par.v4[indices[3]]);
			}
			break;
		case Value::MAT3:
			if(getSize == 1){
				return par.m3[indices[0]];
			}
			break;
		case Value::MAT4:
			if(getSize == 1){
				return par.m4[indices[0]];
			}
			break;
		default:
			break;
	}
	EXIT(&exp, "Unsupported subscript " + exp.member + " for type " + TypeString(par.type) + ".");
}

Value ExpEval::process(const Literal& exp) {
	return exp.val;
}

Value ExpEval::process(const Variable& exp) {
	// If we have local variables (function arguments), they have priority.
	if(!_localScopes.empty()){
		// This should not happen in practice.
		// All variables in function expressions are FunctionVar.
		assert(false);
		const Scope& currentScope = _localScopes.top();
		if(currentScope.hasVar(exp.name)){
			return currentScope.getVar(exp.name);
		}
	}

	// Else check variables declared in the global context.
	if(_globalScope.hasVar(exp.name)){
		return _globalScope.getVar(exp.name);
	}
	// Else undeclared variable.
	EXIT(&exp, "Undefined variable " + exp.name + ".");
}

Value ExpEval::process(FunctionVar& exp) {
	if(exp.hasValue()){
		return exp.value();
	}

	// Else, this is an argument, check in the current local scope if it exists.
	if(!_localScopes.empty()){
		const Scope& currentScope = _localScopes.top();
		if(currentScope.hasVar(exp.name)){
			return currentScope.getVar(exp.name);
		}
	}

	// Can't be in the global context.
	EXIT(&exp, "Undefined variable " + exp.name + ".");
}

Value ExpEval::process(const VariableDef& exp) {
	(void)exp;
	assert(false);
	EXIT(&exp, "Unexpected variable declaration (" + exp.name + " ).");
}

Value ExpEval::process(const FunctionDef& exp)  {
	(void)exp;
	assert(false);
	EXIT(&exp, "Unexpected function declaration (" + exp.name + " ).");
}

Value ExpEval::process(const FunctionCall& exp)  {
	const size_t argCount = exp.args.size();

	// Evaluate all arguments.
	std::vector<Value> argValues;
	argValues.reserve(argCount);
	for(const auto& arg : exp.args){
		argValues.push_back(arg->evaluate(*this));
	}
	if(_failed){
		return false;
	}

	// Check user defined functions
	if(_globalScope.hasFunc(exp.name)){
		// Populate local variable context with arguments
		const auto& funcDef = _globalScope.getFunc(exp.name);
		const size_t expectedCount = funcDef->args.size();
		if(expectedCount != argCount){
			EXIT(&exp, "Incorrect number of arguments for function " + exp.name + ", expected " + std::to_string(expectedCount) + ".");
		}

		Scope& currentScope = _localScopes.emplace();
		for(size_t aid = 0; aid < argCount; ++aid){
			currentScope.setVar(funcDef->args[aid], argValues[aid]);
		}
		const Value res = funcDef->expr->evaluate(*this);
		_localScopes.pop();
		return res;
	}

	if(_stdlib.hasFunc(exp.name)){
		// Check if number of arguments is valid.
		if(!_stdlib.validArgCount(exp.name, exp.args.size())){
			EXIT(&exp, "Incorrect number of arguments for function " + exp.name + ".");
		}
		return _stdlib.eval(exp.name, argValues);
	}

	EXIT(&exp, "Undefined function " + exp.name + ".");
}

FuncSubstitution::FuncSubstitution(const Scope& scope, const FunctionsLibrary& stdlib, const std::vector<std::string>& argNames, const std::string& id)
	: _globalScope(scope), _stdlib(stdlib), _names(argNames), _id(id) {

}

Value FuncSubstitution::process(const Unary& exp)  {
	const Value res = exp.exp->evaluate(*this);
	return res;
}

Value FuncSubstitution::process(const Binary& exp)  {
	const Value l = exp.left->evaluate(*this);
	const Value r = exp.right->evaluate(*this);
	return l.b && r.b;
}

Value FuncSubstitution::process(const Ternary& exp) {
	const Value c = exp.condition->evaluate(*this);
	const Value p = exp.pass->evaluate(*this);
	const Value f = exp.fail->evaluate(*this);
	return c.b && p.b && f.b;
}

Value FuncSubstitution::process(const Member& exp) {
	const Value r = exp.parent->evaluate(*this);
	return r.b;
}

Value FuncSubstitution::process(const Literal& exp) {
	(void)exp;
	return true;
}

Value FuncSubstitution::process(const Variable& exp) {
	// This should not happen in a function declaration.
	assert(false);
	EXIT(&exp, "Unexpected variable " + exp.name + " in function declaration.");
}

Value FuncSubstitution::process(const VariableDef& exp) {
	assert(false);
	EXIT(&exp, "Unexpected variable definition (" + exp.name + ") in function declaration.");
}

Value FuncSubstitution::process(const FunctionDef& exp) {
	assert(false);
	EXIT(&exp, "Unexpected nested function declaration (" + exp.name + ").");
}

Value FuncSubstitution::process(FunctionVar& exp) {
	// If variable in argument list, update its name.
	if(std::find(_names.begin(), _names.end(), exp.name) != _names.end()){
		exp.name.append(_id);
		return true;
	}
	// Else fetch its value from the global variables to bake it.
	if(_globalScope.hasVar(exp.name)){
		exp.setValue(_globalScope.getVar(exp.name));
		return true;
	}
	EXIT(&exp, "Undefined variable " + exp.name + ".");
}

Value FuncSubstitution::process(const FunctionCall& exp)  {
	// Always evaluate arguments first.
	bool res = true;
	for(auto& arg : exp.args){
		const Value r = arg->evaluate(*this);
		res = r.b && res;
	}

	if(!res){
		return res;
	}

	// Then check existence.
	if(_globalScope.hasFunc(exp.name)){
		const size_t expectedCount = _globalScope.getFunc(exp.name)->args.size();
		if(expectedCount != exp.args.size()){
			EXIT(&exp, "Incorrect number of arguments for function " + exp.name + " (expected " + std::to_string(expectedCount) + ").");
		}
		return true;
	}

	if(_stdlib.hasFunc(exp.name)){
		// Check if number of arguments is valid.
		if(!_stdlib.validArgCount(exp.name, exp.args.size())){
			EXIT(&exp, "Incorrect number of arguments for function " + exp.name + ".");
		}
		return true;
	}
	EXIT(&exp, "Undefined function " + exp.name + ".");
}
