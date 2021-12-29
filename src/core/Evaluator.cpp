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
		args += (aid != 0 ? "," : "") + arg->name;
	}
	return exp.name + "( " + args + " ) = " + exp.expr->evaluate(*this).str;
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

#define EXIT(exp, msg) _context.registerError(nullptr, msg); return Value("fail");

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
	return v;
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
	return v;
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
	return v;
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
	return v;
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
	return v;
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
	return Value("fail");
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
	return Value("fail");
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
	return Value("fail");
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
	return Value("false");
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
	return Value("false");
}

Value ExpEval::process(const Binary& exp)  {
	const Value left = exp.left->evaluate(*this);
	const Value right = exp.right->evaluate(*this);

	// Target type
	const Value::Type finalType = std::max(left.type, right.type);
	assert(finalType < Value::Type::STRING);

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
	Value cond = exp.condition->evaluate(*this);
	if(cond.type != Value::BOOL){
		EXIT(exp, "Condition is not boolean.");
	}
	if(cond.b){
		return exp.pass->evaluate(*this);
	}
	return exp.fail->evaluate(*this);

}

Value ExpEval::process(const Member& exp) {
	Value par = exp.parent->evaluate(*this);

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
	EXIT(exp, "Item has no member.");
}

Value ExpEval::process(const Literal& exp) {
	return exp.val;
}

Value ExpEval::process(const Variable& exp) {
	// TODO: fetch from context.
	return Value("unknown");
}

Value ExpEval::process(const VariableDef& exp) {
	assert(false);
	return Value("unknown");
}

Value ExpEval::process(const FunctionDef& exp)  {
	assert(false);
	return Value("unknown");
}

Value ExpEval::process(const FunctionCall& exp)  {
	std::vector<Value> args;
	args.reserve(exp.args.size());
	for(const auto& arg : exp.args){
		args.push_back(arg->evaluate(*this));
	}

	// Temporary hack for basic constructors.

	bool succ;
	if(exp.name == "vec4"){
		const size_t argCount = args.size();
		if(argCount == 1){
			switch (args[0].type) {
				case Value::VEC4:
					return args[0].vec;

				case Value::FLOAT:
				case Value::INTEGER:
				{
					const Value conv = args[0].convert(Value::FLOAT, succ);
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
				args[i] = args[i].convert(Value::FLOAT, succ);
			}
			if(succ){
				return glm::vec4(args[0].f, args[1].f, args[2].f, args[3].f);
			}
		}
		return Value("fail");
	}
	if(exp.name == "mat4"){
		const size_t argCount = args.size();
		if(argCount == 1){
			switch (args[0].type) {
				case Value::MAT4:
					return args[0].mat;
				case Value::FLOAT:
				case Value::INTEGER:
				{
					const Value conv = args[0].convert(Value::FLOAT, succ);
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
				args[i] = args[i].convert(Value::VEC4, succ);
			}
			if(succ){
				return glm::mat4(args[0].vec, args[1].vec, args[2].vec, args[3].vec);
			}
		} else if(argCount == 16){
			for(int i = 0; i < 16; ++i){
				args[i] = args[i].convert(Value::FLOAT, succ);
			}
			if(succ){
				glm::mat4 res;
				for(int i = 0; i < 4; ++i){
					for(int j = 0; j < 4; ++j){
						res[i][j] = args[4*i+j].f;
					}
				}
				return res;
			}
		}
		return Value("fail");
	}
	// TODO: check global functions
	// TODO: check local functions
	// TODO: evaluate all arguments
	// TODO: populate local variable context with arguments
	return Value("unknown");
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
