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

#define EXIT(exp, msg) _context.registerError(nullptr, msg); Log::Error() << msg << std::endl; return {};

ExpEval::ExpEval(Evaluator& context) : _context(context){}


bool unaryBool(Operator op, bool v, bool& status){
	status = true;
	switch(op){
		case Operator::BitNot:
			return !v;
		case Operator::BoolNot:
			return !v;
		default:
			break;
	}
	status = false;
	return {};
}

long long unaryInt(Operator op, long long v, bool& status){
	status = true;
	switch(op){
		case Operator::Plus:
			return v;
		case Operator::Minus:
			return -v;
		case Operator::BitNot:
			return ~v;
		default:
			break;
	}
	status = false;
	return {};
}

double unaryFloat(Operator op, double v, bool& status){
	status = true;
	switch(op){
		case Operator::Plus:
			return v;
		case Operator::Minus:
			return -v;
		default:
			break;
	}
	status = false;
	return {};
}

glm::vec4 unaryVec(Operator op, const glm::vec4& v, bool& status){
	status = true;
	switch(op){
		case Operator::Plus:
			return v;
		case Operator::Minus:
			return -v;
		default:
			break;
	}
	status = false;
	return {};
}

glm::mat4 unaryMat(Operator op, const glm::mat4& v, bool& status){
	status = true;
	switch(op){
		case Operator::Plus:
			return v;
		case Operator::Minus:
			return -v;
		default:
			break;
	}
	status = false;
	return {};
}

Value ExpEval::process(const Unary& exp)  {
	Value val = exp.exp->evaluate(*this);
	bool status = false;
	switch(val.type){
		case Value::BOOL:
			val.b = unaryBool(exp.op, val.b, status);
		case Value::INTEGER:
			val.i = unaryInt(exp.op, val.i, status);
			break;
		case Value::FLOAT:
			val.f = unaryFloat(exp.op, val.f, status);
			break;
		case Value::VEC4:
			val.vec = unaryVec(exp.op, val.vec, status);
			break;
		case Value::MAT4:
			val.mat = unaryMat(exp.op, val.mat, status);
			break;
		default:
			break;
	}

	if(!status){
		EXIT(exp, "Unsupported unary operator");
	}
	return val;
}

Value binaryBool(Operator op, bool a, bool b, bool& status){
	status = true;
	switch(op){
		case Operator::Equal:
			return a == b;
		case Operator::Different:
			return a != b;
		case Operator::BoolOr:
			return a || b;
		case Operator::BoolAnd:
			return a && b;
		case Operator::BoolXor:
			return (!a && b) || (a && !b);
		default:
			break;
	}
	status = false;
	return {};
}

Value binaryInt(Operator op, long long a, long long b, bool& status){
	status = true;
	switch(op){
		case Operator::Plus:
			return a+b;
		case Operator::Minus:
			return a-b;
		case Operator::Product:
			return a*b;
		case Operator::Divide:
			return double(a)/double(b);
		case Operator::Power:
			return (long long)(pow(a, b));
		case Operator::Modulo:
			return a % b;
		case Operator::ShiftLeft:
			return a << b;
		case Operator::ShiftRight:
			return a >> b;
		case Operator::LessThan:
			return a < b;
		case Operator::GreaterThan:
			return a > b;
		case Operator::LessThanEqual:
			return a <= b;
		case Operator::GreaterThanEqual:
			return a >= b;
		case Operator::Equal:
			return a == b;
		case Operator::Different:
			return a != b;
		case Operator::BitOr:
			return a | b;
		case Operator::BitAnd:
			return a & b;
		case Operator::BitXor:
			return a ^ b;
		default:
			break;
	}
	status = false;
	return {};
}

Value binaryFloat(Operator op, double a, double b, bool& status){
	status = true;
	switch(op){
		case Operator::Plus:
			return a+b;
		case Operator::Minus:
			return a-b;
		case Operator::Product:
			return a*b;
		case Operator::Divide:
			return a/b;
		case Operator::Power:
			return glm::pow(a, b);
		case Operator::Modulo:
			return glm::mod(a, b);
		case Operator::LessThan:
			return a < b;
		case Operator::GreaterThan:
			return a > b;
		case Operator::LessThanEqual:
			return a <= b;
		case Operator::GreaterThanEqual:
			return a >= b;
		case Operator::Equal:
			return a == b;
		case Operator::Different:
			return a != b;
		default:
			break;
	}
	status = false;
	return {};
}

Value binaryVec(Operator op, const glm::vec4& a, const glm::vec4& b, bool& status){
	status = true;
	switch(op){
		case Operator::Plus:
			return a+b;
		case Operator::Minus:
			return a-b;
		case Operator::Product:
			return a*b;
		case Operator::Divide:
			return a/b;
		case Operator::Power:
			return glm::pow(a, b);
		case Operator::Modulo:
			return glm::mod(a, b);
		case Operator::LessThan:
			return glm::all(glm::lessThan(a, b));
		case Operator::GreaterThan:
			return glm::all(glm::greaterThan(a, b));
		case Operator::LessThanEqual:
			return glm::all(glm::lessThanEqual(a, b));
		case Operator::GreaterThanEqual:
			return glm::all(glm::greaterThanEqual(a, b));
		case Operator::Equal:
			return glm::all(glm::equal(a, b));
		case Operator::Different:
			return glm::any(glm::notEqual(a, b));
		default:
			break;
	}
	status = false;
	return {};
}

Value binaryMat(Operator op, const glm::mat4& a, const glm::mat4& b, bool& status){
	status = true;
	switch(op){
		case Operator::Plus:
			return a+b;
		case Operator::Minus:
			return a-b;
		case Operator::Product:
			return a*b;
		case Operator::Divide:
			return a/b;
		case Operator::Equal:
			return a == b;
		case Operator::Different:
			return a != b;
		default:
			break;
	}
	status = false;
	return {};
}

Value ExpEval::process(const Binary& exp)  {
	const Value left = exp.left->evaluate(*this);
	const Value right = exp.right->evaluate(*this);

	// Target type
	const Value::Type finalType = std::max(left.type, right.type);

	bool res0, res1;
	const Value leftConv = left.convert(finalType, res0);
	const Value rightConv = right.convert(finalType, res1);
	// TODO: special case for mat*vec
	if(!res0 || !res1){
		EXIT(exp, "Can't convert");
	}

	bool status = false;
	const Operator op = exp.op;
	Value result(0ll);
	switch (finalType) {
		case Value::BOOL:
			result = binaryBool(op, leftConv.b, rightConv.b, status);
			break;
		case Value::INTEGER:
			result = binaryInt(op, leftConv.i, rightConv.i, status);
			break;
		case Value::FLOAT:
			result = binaryFloat(op, leftConv.f, rightConv.f, status);
			break;
		case Value::VEC4:
			result = binaryVec(op, leftConv.vec, rightConv.vec, status);
			break;
		case Value::MAT4:
			result = binaryMat(op, leftConv.mat, rightConv.mat, status);
			break;

		default:
			break;
	}
	if(!status){
		EXIT(exp, "Unsupported unary operator");
	}
	return result;
}

Value ExpEval::process(const Ternary& exp) {
	const Value cond = exp.condition->evaluate(*this);

	// Cast to bool.
	bool success = true;
	const Value condBool = cond.convert(Value::BOOL, success);
	if(!success){
		EXIT(exp, "Condition could not be converted to a boolean.");
	}

	// Partial evaluation.
	if(condBool.b){
		return exp.pass->evaluate(*this);
	}
	return exp.fail->evaluate(*this);

}

Value ExpEval::process(const Member& exp) {
	Value par = exp.parent->evaluate(*this);
	// TODO: more complex getters
	switch (par.type) {
		case Value::MAT4:
			if(exp.member == "x"){
				return par.mat[0];
			}
			if(exp.member == "y"){
				return par.mat[1];
			}
			if(exp.member == "z"){
				return par.mat[2];
			}
			if(exp.member == "w"){
				return par.mat[3];
			}
			break;
		case Value::VEC4:
			if(exp.member == "x"){
				return par.vec.x;
			}
			if(exp.member == "y"){
				return par.vec.y;
			}
			if(exp.member == "z"){
				return par.vec.z;
			}
			if(exp.member == "w"){
				return par.vec.w;
			}
			break;

		default:
			break;
	}
	EXIT(exp, "Item has no member " + exp.member + ".");
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
	const Scope& globalScope = _context.globalScope();
	if(globalScope.hasVar(exp.name)){
		return globalScope.getVar(exp.name);
	}
	// Else undeclared variable.
	EXIT(&exp, "Variable " + exp.name + " doesn't exist.");
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
	EXIT(&exp, "Variable " + exp.name + " doesn't exist.");
}

Value ExpEval::process(const VariableDef& exp) {
	(void)exp;
	assert(false);
	return {};
}

Value ExpEval::process(const FunctionDef& exp)  {
	(void)exp;
	assert(false);
	return {};
}

Value ExpEval::process(const FunctionCall& exp)  {
	const size_t argCount = exp.args.size();

	// Evaluate all arguments.
	std::vector<Value> argValues;
	argValues.reserve(argCount);
	for(const auto& arg : exp.args){
		argValues.push_back(arg->evaluate(*this));
	}

	// Check local functions
	if(_context.globalScope().hasFunc(exp.name)){
		// Populate local variable context with arguments
		const auto& funcDef = _context.globalScope().getFunc(exp.name);
		if(funcDef->args.size() != argCount){
			EXIT(exp, "Incorrect number of arguments for function " + exp.name);
		}

		Scope& currentScope = _localScopes.emplace();
		for(size_t aid = 0; aid < argCount; ++aid){
			currentScope.setVar(funcDef->args[aid], argValues[aid]);
		}
		const Value res = funcDef->expr->evaluate(*this);
		_localScopes.pop();
		return res;
	}

	// TODO: check global functions
	// Temporary hack for basic constructors.
	bool succ;
	if(exp.name == "vec4"){
		if(argCount == 1){
			switch (argValues[0].type) {
				case Value::VEC4:
					return argValues[0].vec;

				case Value::FLOAT:
				case Value::INTEGER:
				{
					const Value conv = argValues[0].convert(Value::FLOAT, succ);
					if(succ){
						return glm::vec4(conv.f);
					}
					break;
				}
				default:
					break;
			}
		} else if(argCount == 4){
			for(int i = 0; i < 4; ++i){
				argValues[i] = argValues[i].convert(Value::FLOAT, succ);
			}
			if(succ){
				return glm::vec4(argValues[0].f, argValues[1].f, argValues[2].f, argValues[3].f);
			}
		}
		return {};
	}
	if(exp.name == "mat4"){
		if(argCount == 1){
			switch (argValues[0].type) {
				case Value::MAT4:
					return argValues[0].mat;
				case Value::FLOAT:
				case Value::INTEGER:
				{
					const Value conv = argValues[0].convert(Value::FLOAT, succ);
					if(succ){
						return glm::mat4(conv.f);
					}
					break;
				}
				default:
					break;
			}
		} else if(argCount == 4){
			for(int i = 0; i < 4; ++i){
				argValues[i] = argValues[i].convert(Value::VEC4, succ);
			}
			if(succ){
				return glm::mat4(argValues[0].vec, argValues[1].vec, argValues[2].vec, argValues[3].vec);
			}
		} else if(argCount == 16){
			for(int i = 0; i < 16; ++i){
				argValues[i] = argValues[i].convert(Value::FLOAT, succ);
			}
			if(succ){
				glm::mat4 res;
				for(int i = 0; i < 4; ++i){
					for(int j = 0; j < 4; ++j){
						res[i][j] = argValues[4*i+j].f;
					}
				}
				return res;
			}
		}
		return {};
	}
	return {};
}

FuncSubstitution::FuncSubstitution(Evaluator& context, const std::vector<std::string>& argNames, const std::string& id)
	: _context(context), _names(argNames), _id(id) {

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
	(void)exp;
	// This should not happen in a function declaration.
	assert(false);
	return false;
}

Value FuncSubstitution::process(const VariableDef& exp) {
	(void)exp;
	assert(false);
	return false;
}

Value FuncSubstitution::process(const FunctionDef& exp) {
	(void)exp;
	assert(false);
	return false;
}

Value FuncSubstitution::process(FunctionVar& exp) {
	// If variable in argument list, update its name.
	if(std::find(_names.begin(), _names.end(), exp.name) != _names.end()){
		exp.name.append(_id);
		return true;
	}
	// Else fetch its value from the global variables to bake it.
	const Scope& globalScope = _context.globalScope();
	if(globalScope.hasVar(exp.name)){
		exp.setValue(globalScope.getVar(exp.name));
		return true;
	}
	EXIT(nullptr, "Undefined variable " + exp.name);
	return false;
}

Value FuncSubstitution::process(const FunctionCall& exp)  {
	bool res = true;
	for(auto& arg : exp.args){
		const Value r = arg->evaluate(*this);
		res = r.b && res;
	}
	return res;
}


void Evaluator::registerError(const Expression::Ptr& exp, const std::string& msg){
	if(!_failed){
		_failedMessage = msg;
		_failedExpression = exp;
	}
	_failed = true;
}

std::string Evaluator::log(){
	if(_tree == nullptr){
		return "Empty tree";
	}
	
	ExpLogger logger;
	Value finalStr = _tree->evaluate(logger);
	return finalStr.str;
}

Value Evaluator::eval(){
	if(_tree == nullptr){
		return Value("");
	}
	ExpEval evaluator(*this);
	return _tree->evaluate(evaluator);
}

Status Evaluator::substitute(std::vector<std::string>& argNames, const std::string& id){
	if(_tree == nullptr){
		return Status(0, "Empty expression");
	}
	
	FuncSubstitution substitutor(*this, argNames, id);
	const Value res = _tree->evaluate(substitutor);

	// Update names after all substitutions and funcVariable modifications.
	for(std::string& argName : argNames){
		argName.append(id);
	}
	return res.b ? Status() : Status(0, _failedMessage);
}
