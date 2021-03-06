#include "core/Functions.hpp"
#include "core/Evaluator.hpp"
#include "core/Scanner.hpp"
#include "core/Parser.hpp"
#include <array>

void Scope::setVar(const std::string& name, const Value& value){
	_variables[name] = value;
}

bool Scope::hasVar(const std::string& name) const {
	return _variables.count(name) != 0;
}

const Value& Scope::getVar(const std::string& name) const {
	return _variables.at(name);
}

void Scope::setFunc(const std::string& name, const std::shared_ptr<FunctionDef>& value){
	_functions[name] = value;
}

bool Scope::hasFunc(const std::string& name) const {
	return _functions.count(name) != 0;
}

const std::shared_ptr<FunctionDef>& Scope::getFunc(const std::string& name) const {
	return _functions.at(name);
}

#define EXIT(msg) evaluator.registerError(msg, nullptr); return false;

bool allArgs(const std::vector<Value>& args, Value::Type type){
	for(const auto& arg : args){
		if(arg.type != type){
			return false;
		}
	}
	return true;
}

Value FunctionsLibrary::funcClamp(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 3);
	const Value& x = args[0];
	Value a, b;
	if(!args[1].convert(x.type, a) || !args[2].convert(x.type, b)){
		EXIT("Unable to convert all arguments to type " + TypeString(x.type) + ".");
	}
	switch(x.type){
		case Value::BOOL:
			return glm::clamp(x.b, a.b, b.b);
		case Value::INTEGER:
			return glm::clamp(x.i, a.i, b.i);
		case Value::FLOAT:
			return glm::clamp(x.f, a.f, b.f);
		case Value::VEC3:
			return glm::clamp(x.v3, a.v3, b.v3);
		case Value::VEC4:
			return glm::clamp(x.v4, a.v4, b.v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcPow(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 2);
	const Value& x = args[0];
	Value e;
	if(!args[1].convert(x.type, e)){
		EXIT("Unable to convert all arguments to type " + TypeString(x.type) + ".");
	}
	switch(x.type){
		case Value::BOOL:
			return glm::pow(x.b, e.b);
		case Value::INTEGER:
			return glm::pow(x.i, e.i);
		case Value::FLOAT:
			return glm::pow(x.f, e.f);
		case Value::VEC3:
			return glm::pow(x.v3, e.v3);
		case Value::VEC4:
			return glm::pow(x.v4, e.v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcMin(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 2);
	const Value::Type maxType = std::max(args[0].type, args[1].type);
	Value a, b;
	if(!args[0].convert(maxType, a) || !args[1].convert(maxType, b)){
		EXIT("Unable to convert all arguments to type " + TypeString(maxType) + ".");
	}
	switch(maxType){
		case Value::BOOL:
			return glm::min(a.b, b.b);
		case Value::INTEGER:
			return glm::min(a.i, b.i);
		case Value::FLOAT:
			return glm::min(a.f, b.f);
		case Value::VEC3:
			return glm::min(a.v3, b.v3);
		case Value::VEC4:
			return glm::min(a.v4, b.v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcMax(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 2);
	const Value::Type maxType = std::max(args[0].type, args[1].type);
	Value a, b;
	if(!args[0].convert(maxType, a) || !args[1].convert(maxType, b)){
		EXIT("Unable to convert all arguments to type " + TypeString(maxType) + ".");
	}
	switch(maxType){
		case Value::BOOL:
			return glm::max(a.b, b.b);
		case Value::INTEGER:
			return glm::max(a.i, b.i);
		case Value::FLOAT:
			return glm::max(a.f, b.f);
		case Value::VEC3:
			return glm::max(a.v3, b.v3);
		case Value::VEC4:
			return glm::max(a.v4, b.v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcSaturate(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	const Value& x = args[0];
	switch(x.type){
		case Value::BOOL:
			return x.b;
		case Value::INTEGER:
			return glm::clamp(x.i, 0ll, 1ll);
		case Value::FLOAT:
			return glm::clamp(x.f, 0.0, 1.0);
		case Value::VEC3:
			return glm::clamp(x.v3, glm::vec3(0.0), glm::vec3(1.0));
		case Value::VEC4:
			return glm::clamp(x.v4, glm::vec4(0.0), glm::vec4(1.0));
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcBin(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	if(args[0].type == Value::INTEGER){
		evaluator.setBase(Format::BASE_2_FLAG);
		return args[0];
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcHex(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	if(args[0].type == Value::INTEGER){
		evaluator.setBase(Format::BASE_16_FLAG);
		return args[0];
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcOct(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	if(args[0].type == Value::INTEGER){
		evaluator.setBase(Format::BASE_8_FLAG);
		return args[0];
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcDec(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	if(args[0].type == Value::INTEGER){
		evaluator.setBase(Format::BASE_10_FLAG);
		return args[0];
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcCos(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::cos(args[0].b);
		case Value::INTEGER:
			return glm::cos(args[0].i);
		case Value::FLOAT:
			return glm::cos(args[0].f);
		case Value::VEC3:
			return glm::cos(args[0].v3);
		case Value::VEC4:
			return glm::cos(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcSin(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::sin(args[0].b);
		case Value::INTEGER:
			return glm::sin(args[0].i);
		case Value::FLOAT:
			return glm::sin(args[0].f);
		case Value::VEC3:
			return glm::sin(args[0].v3);
		case Value::VEC4:
			return glm::sin(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcTan(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::tan(args[0].b);
		case Value::INTEGER:
			return glm::tan(args[0].i);
		case Value::FLOAT:
			return glm::tan(args[0].f);
		case Value::VEC3:
			return glm::tan(args[0].v3);
		case Value::VEC4:
			return glm::tan(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcAcos(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::acos(args[0].b);
		case Value::INTEGER:
			return glm::acos(args[0].i);
		case Value::FLOAT:
			return glm::acos(args[0].f);
		case Value::VEC3:
			return glm::acos(args[0].v3);
		case Value::VEC4:
			return glm::acos(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcAsin(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::asin(args[0].b);
		case Value::INTEGER:
			return glm::asin(args[0].i);
		case Value::FLOAT:
			return glm::asin(args[0].f);
		case Value::VEC3:
			return glm::asin(args[0].v3);
		case Value::VEC4:
			return glm::asin(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcAtan(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1 || args.size() == 2);
	if(args.size() == 1){
		switch(args[0].type){
			case Value::BOOL:
				return glm::atan(args[0].b);
			case Value::INTEGER:
				return glm::atan(args[0].i);
			case Value::FLOAT:
				return glm::atan(args[0].f);
			case Value::VEC3:
				return glm::atan(args[0].v3);
			case Value::VEC4:
				return glm::atan(args[0].v4);
			default:
				break;
		}
	} else {
		if(args[0].type != args[1].type){
			EXIT("Arguments have incompatible types: " + TypeString(args[0].type) + " and " + TypeString(args[1].type) + ".");
		}
		switch(args[0].type){
			case Value::FLOAT:
				return glm::atan(args[0].f, args[1].f);
			case Value::VEC3:
				return glm::atan(args[0].v3, args[1].v3);
			case Value::VEC4:
				return glm::atan(args[0].v4, args[1].v4);
			default:
				break;
		}
	}

	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcExp(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::exp(args[0].b);
		case Value::INTEGER:
			return glm::exp(args[0].i);
		case Value::FLOAT:
			return glm::exp(args[0].f);
		case Value::VEC3:
			return glm::exp(args[0].v3);
		case Value::VEC4:
			return glm::exp(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcLog(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::log(args[0].b);
		case Value::INTEGER:
			return glm::log(args[0].i);
		case Value::FLOAT:
			return glm::log(args[0].f);
		case Value::VEC3:
			return glm::log(args[0].v3);
		case Value::VEC4:
			return glm::log(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcExp2(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::exp2(args[0].b);
		case Value::INTEGER:
			return glm::exp2(args[0].i);
		case Value::FLOAT:
			return glm::exp2(args[0].f);
		case Value::VEC3:
			return glm::exp2(args[0].v3);
		case Value::VEC4:
			return glm::exp2(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcLog2(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::FLOAT:
			return glm::log2(args[0].f);
		case Value::VEC3:
			return glm::log2(args[0].v3);
		case Value::VEC4:
			return glm::log2(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcSqrt(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::sqrt(args[0].b);
		case Value::INTEGER:
			return glm::sqrt(args[0].i);
		case Value::FLOAT:
			return glm::sqrt(args[0].f);
		case Value::VEC3:
			return glm::sqrt(args[0].v3);
		case Value::VEC4:
			return glm::sqrt(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcXor(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 2);
	if(allArgs(args, Value::INTEGER)){
		return args[0].i ^ args[1].i;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcFloor(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::floor(args[0].b);
		case Value::INTEGER:
			return glm::floor(args[0].i);
		case Value::FLOAT:
			return glm::floor(args[0].f);
		case Value::VEC3:
			return glm::floor(args[0].v3);
		case Value::VEC4:
			return glm::floor(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcCeil(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::ceil(args[0].b);
		case Value::INTEGER:
			return glm::ceil(args[0].i);
		case Value::FLOAT:
			return glm::ceil(args[0].f);
		case Value::VEC3:
			return glm::ceil(args[0].v3);
		case Value::VEC4:
			return glm::ceil(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcFract(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::FLOAT:
			return glm::fract(args[0].f);
		case Value::VEC3:
			return glm::fract(args[0].v3);
		case Value::VEC4:
			return glm::fract(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcMix(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 3);
	// Based on type of first argument.
	Value y, t;
	const Value& x = args[0];
	if(!args[1].convert(x.type, y) || !args[2].convert(x.type, t)){
		EXIT("Unable to convert all arguments to type " + TypeString(x.type) + ".");
	}
	switch(x.type){
		case Value::FLOAT:
			return glm::mix(x.f, y.f, t.f);
		case Value::VEC3:
			return glm::mix(x.v3, y.v3, t.v3);
		case Value::VEC4:
			return glm::mix(x.v4, y.v4, t.v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcAbs(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::abs(args[0].b);
		case Value::INTEGER:
			return glm::abs(args[0].i);
		case Value::FLOAT:
			return glm::abs(args[0].f);
		case Value::VEC3:
			return glm::abs(args[0].v3);
		case Value::VEC4:
			return glm::abs(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcInversesqrt(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::INTEGER:
			return glm::inversesqrt(double(args[0].i));
		case Value::FLOAT:
			return glm::inversesqrt(args[0].f);
		case Value::VEC3:
			return glm::inversesqrt(args[0].v3);
		case Value::VEC4:
			return glm::inversesqrt(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcRcp(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::INTEGER:
			return 1.0f/double(args[0].i);
		case Value::FLOAT:
			return 1.0f/args[0].f;
		case Value::VEC3:
			return 1.0f/args[0].v3;
		case Value::VEC4:
			return 1.0f/args[0].v4;
		case Value::MAT3:
			return 1.0f/args[0].m3;
		case Value::MAT4:
			return 1.0f/args[0].m4;
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcSign(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	switch(args[0].type){
		case Value::INTEGER:
			return glm::sign(args[0].i);
		case Value::FLOAT:
			return glm::sign(args[0].f);
		case Value::VEC3:
			return glm::sign(args[0].v3);
		case Value::VEC4:
			return glm::sign(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcMod(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 2);
	Value e;
	const Value& x = args[0];
	if(!args[1].convert(x.type, e)){
		EXIT("Unable to convert all arguments to type " + TypeString(x.type) + ".");
	}
	switch(x.type){
		case Value::FLOAT:
			return glm::mod(x.f, e.f);
		case Value::VEC3:
			return glm::mod(x.v3, e.v3);
		case Value::VEC4:
			return glm::mod(x.v4, e.v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcStep(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 2);
	Value e;
	const Value& x = args[1];
	if(!args[0].convert(x.type, e)){
		EXIT("Unable to convert all arguments to type " + TypeString(x.type) + ".");
	}
	switch(x.type){
		case Value::FLOAT:
			return glm::step(e.f, x.f);
		case Value::VEC3:
			return glm::step(e.v3, x.v3);
		case Value::VEC4:
			return glm::step(e.v4, x.v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcSmoothstep(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 3);
	// Based on type of third argument.
	Value e0, e1;
	const Value& x = args[2];
	if(!args[0].convert(x.type, e0) || !args[1].convert(x.type, e1)){
		EXIT("Unable to convert all arguments to type " + TypeString(x.type) + ".");
	}
	switch(x.type){
		case Value::FLOAT:
			return glm::smoothstep(e0.f, e1.f, x.f);
		case Value::VEC3:
			return glm::smoothstep(e0.v3, e1.v3, x.v3);
		case Value::VEC4:
			return glm::smoothstep(e0.v4, e1.v4, x.v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcLength(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	if(args[0].type == Value::VEC3){
		return glm::length(args[0].v3);
	}
	if(args[0].type == Value::VEC4){
		return glm::length(args[0].v4);
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcDistance(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 2);
	if(allArgs(args, Value::VEC3)){
		return glm::distance(args[0].v3, args[1].v3);
	}
	if(allArgs(args, Value::VEC4)){
		return glm::distance(args[0].v4, args[1].v4);
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcDot(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 2);
	if(allArgs(args, Value::VEC3)){
		return glm::dot(args[0].v3, args[1].v3);
	}
	if(allArgs(args, Value::VEC4)){
		return glm::dot(args[0].v4, args[1].v4);
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcCross(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 2);
	if(allArgs(args, Value::VEC3)){
		return glm::cross(args[0].v3, args[1].v3);
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcNormalize(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	if(args[0].type == Value::VEC3){
		return glm::normalize(args[0].v3);
	}
	if(args[0].type == Value::VEC4){
		return glm::normalize(args[0].v4);
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcReflect(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 2);
	if(allArgs(args, Value::VEC3)){
		return glm::reflect(args[0].v3, args[1].v3);
	}
	if(allArgs(args, Value::VEC4)){
		return glm::reflect(args[0].v4, args[1].v4);
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcRefract(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 3);
	if(args[0].type == Value::VEC3 && args[1].type == Value::VEC3 && args[2].type == Value::FLOAT){
		return glm::refract(args[0].v3, args[1].v3, float(args[2].f));
	}
	if(args[0].type == Value::VEC4 && args[1].type == Value::VEC4 && args[2].type == Value::FLOAT){
		return glm::refract(args[0].v4, args[1].v4, float(args[2].f));
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcInverse(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::MAT3:
			return glm::inverse(args[0].m3);
		case Value::MAT4:
			return glm::inverse(args[0].m4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcTranspose(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::MAT3:
			return glm::transpose(args[0].m3);
		case Value::MAT4:
			return glm::transpose(args[0].m4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcMatrixCompMult(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 2);
	if(allArgs(args, Value::MAT4)){
		return glm::matrixCompMult(args[0].m4, args[1].m4);
	}
	if(allArgs(args, Value::MAT3)){
		return glm::matrixCompMult(args[0].m3, args[1].m3);
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcRadians(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::FLOAT:
			return glm::radians(args[0].f);
		case Value::VEC3:
			return glm::radians(args[0].v3);
		case Value::VEC4:
			return glm::radians(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcDegrees(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::FLOAT:
			return glm::degrees(args[0].f);
		case Value::VEC3:
			return glm::degrees(args[0].v3);
		case Value::VEC4:
			return glm::degrees(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcSinh(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::sinh(args[0].b);
		case Value::INTEGER:
			return glm::sinh(args[0].i);
		case Value::FLOAT:
			return glm::sinh(args[0].f);
		case Value::VEC3:
			return glm::sinh(args[0].v3);
		case Value::VEC4:
			return glm::sinh(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcCosh(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::cosh(args[0].b);
		case Value::INTEGER:
			return glm::cosh(args[0].i);
		case Value::FLOAT:
			return glm::cosh(args[0].f);
		case Value::VEC3:
			return glm::cosh(args[0].v3);
		case Value::VEC4:
			return glm::cosh(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcTanh(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::tanh(args[0].b);
		case Value::INTEGER:
			return glm::tanh(args[0].i);
		case Value::FLOAT:
			return glm::tanh(args[0].f);
		case Value::VEC3:
			return glm::tanh(args[0].v3);
		case Value::VEC4:
			return glm::tanh(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcAsinh(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::asinh(args[0].b);
		case Value::INTEGER:
			return glm::asinh(args[0].i);
		case Value::FLOAT:
			return glm::asinh(args[0].f);
		case Value::VEC3:
			return glm::asinh(args[0].v3);
		case Value::VEC4:
			return glm::asinh(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcAcosh(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::acosh(args[0].b);
		case Value::INTEGER:
			return glm::acosh(args[0].i);
		case Value::FLOAT:
			return glm::acosh(args[0].f);
		case Value::VEC3:
			return glm::acosh(args[0].v3);
		case Value::VEC4:
			return glm::acosh(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}


Value FunctionsLibrary::funcAtanh(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::atanh(args[0].b);
		case Value::INTEGER:
			return glm::atanh(args[0].i);
		case Value::FLOAT:
			return glm::atanh(args[0].f);
		case Value::VEC3:
			return glm::atanh(args[0].v3);
		case Value::VEC4:
			return glm::atanh(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcRound(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::round(args[0].b);
		case Value::INTEGER:
			return glm::round(args[0].i);
		case Value::FLOAT:
			return glm::round(args[0].f);
		case Value::VEC3:
			return glm::round(args[0].v3);
		case Value::VEC4:
			return glm::round(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcTrunc(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::BOOL:
			return glm::trunc(args[0].b);
		case Value::INTEGER:
			return glm::trunc(args[0].i);
		case Value::FLOAT:
			return glm::trunc(args[0].f);
		case Value::VEC3:
			return glm::trunc(args[0].v3);
		case Value::VEC4:
			return glm::trunc(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcOuterProduct(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 2);
	if(allArgs(args, Value::VEC3)){
		return glm::outerProduct(args[0].v3, args[1].v3);
	}
	if(allArgs(args, Value::VEC4)){
		return glm::outerProduct(args[0].v4, args[1].v4);
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcDeterminant(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	if(args[0].type == Value::MAT3){
		return glm::determinant(args[0].m3);
	}
	if(args[0].type == Value::MAT4){
		return glm::determinant(args[0].m4);
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcLookAt(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 3);
	if(allArgs(args, Value::VEC3)){
		return glm::lookAt(args[0].v3, args[1].v3, args[2].v3);
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcPerspective(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 4);
	if(allArgs(args, Value::FLOAT)){
		return glm::mat4(glm::perspective(args[0].f, args[1].f, args[2].f, args[3].f));
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcOrthographic(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 6);
	if(allArgs(args, Value::FLOAT)){
		return glm::mat4(glm::ortho(args[0].f, args[1].f, args[2].f, args[3].f, args[4].f, args[5].f));
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcAxisRotationMat(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 2);
	if(args[0].type == Value::FLOAT && args[1].type == Value::VEC3){
		return glm::rotate(glm::mat4(1.0f), float(args[0].f), args[1].v3);
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcTranslationMat(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	if(args[0].type == Value::VEC3){
		return glm::translate(glm::mat4(1.0f), args[0].v3);
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::funcScalingMat(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	assert(args.size() == 1);
	if(args[0].type == Value::VEC3){
		return glm::scale(glm::mat4(1.0f), args[0].v3);
	} else if(args[0].type == Value::FLOAT){
		return glm::scale(glm::mat4(1.0f), glm::vec3(float(args[0].f)));
	}
	EXIT("Unsupported type " + TypeString(args[0].type) + " for function " + name + ".");
}

Value FunctionsLibrary::constructorVec3(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	if(args.size() == 1){
		switch (args[0].type) {
			case Value::INTEGER:
				return glm::vec3(float(args[0].i));
			case Value::FLOAT:
				return glm::vec3(float(args[0].f));
			case Value::VEC3:
				return args[0].v3;
			case Value::VEC4:
				return glm::vec3(args[0].v4);
			default:
				break;
		}
	} else if(args.size() == 3){
		std::array<Value, 3> cargs;
		for(int i = 0; i < 3; ++i){
			if(!args[i].convert(Value::FLOAT, cargs[i])){
				EXIT("Unable to convert argument to type float.");
			}
		}
		return glm::vec3(cargs[0].f, cargs[1].f, cargs[2].f);
	}
	EXIT("Unsupported " + name + " constructor.");
}

Value FunctionsLibrary::constructorVec4(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	if(args.size() == 1){
		switch (args[0].type) {
			case Value::INTEGER:
				return glm::vec4(float(args[0].i));
			case Value::FLOAT:
				return glm::vec4(float(args[0].f));
			case Value::VEC4:
				return args[0].v4;
			default:
				break;
		}
	} else if(args.size() == 2){
		if(args[0].type == Value::FLOAT && args[1].type == Value::VEC3){
			return glm::vec4(args[0].f, args[1].v3);
		}
		if(args[0].type == Value::VEC3 && args[1].type == Value::FLOAT){
			return glm::vec4(args[0].v3, args[1].f);
		}
	} else if(args.size() == 4){
		std::array<Value, 4> cargs;
		for(int i = 0; i < 4; ++i){
			if(!args[i].convert(Value::FLOAT, cargs[i])){
				EXIT("Unable to convert argument to type float.");
			}
		}
		return glm::vec4(cargs[0].f, cargs[1].f, cargs[2].f, cargs[3].f);
	}
	EXIT("Unsupported " + name + " constructor.");
}

Value FunctionsLibrary::constructorMat3(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	if(args.size() == 1){
		switch (args[0].type) {
			case Value::INTEGER:
				return glm::mat3(float(args[0].i));
			case Value::FLOAT:
				return glm::mat3(float(args[0].f));
			case Value::MAT3:
				return args[0].m3;
			case Value::MAT4:
				return glm::mat3(args[0].m4);
			default:
				break;
		}

	} else if(args.size() == 3){
		std::array<Value, 3> cargs;
		for(int i = 0; i < 3; ++i){
			if(!args[i].convert(Value::VEC3, cargs[i])){
				EXIT("Unable to convert argument to type vec3.");
			}
		}
		return glm::mat3(cargs[0].v3, cargs[1].v3, cargs[2].v3);

	} else if(args.size() == 9){
		std::array<Value, 9> cargs;
		for(int i = 0; i < 9; ++i){
			if(!args[i].convert(Value::FLOAT, cargs[i])){
				EXIT("Unable to convert argument to type float.");
			}
		}
		return glm::mat3(cargs[0].f, cargs[1].f, cargs[2].f,
						 cargs[3].f, cargs[4].f, cargs[5].f,
						 cargs[6].f, cargs[7].f, cargs[8].f);

	}
	EXIT("Unsupported " + name + " constructor.");
}

Value FunctionsLibrary::constructorMat4(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name){
	if(args.size() == 1){
		switch (args[0].type) {
			case Value::INTEGER:
				return glm::mat4(float(args[0].i));
			case Value::FLOAT:
				return glm::mat4(float(args[0].f));
			case Value::MAT3:
				return glm::mat4(args[0].m3);
			case Value::MAT4:
				return args[0].m4;
			default:
				break;
		}

	} else if(args.size() == 4){
		std::array<Value, 4> cargs;
		for(int i = 0; i < 4; ++i){
			if(!args[i].convert(Value::VEC4, cargs[i])){
				EXIT("Unable to convert argument to type vec4.");
			}
		}
		return glm::mat4(cargs[0].v4, cargs[1].v4, cargs[2].v4, cargs[3].v4);

	} else if(args.size() == 16){
		std::array<Value, 16> cargs;
		for(int i = 0; i < 16; ++i){
			if(!args[i].convert(Value::FLOAT, cargs[i])){
				EXIT("Unable to convert argument to type float.");
			}
		}
		return glm::mat4(cargs[0].f, cargs[1].f, cargs[2].f, cargs[3].f,
						 cargs[4].f, cargs[5].f, cargs[6].f, cargs[7].f,
						 cargs[8].f, cargs[9].f, cargs[10].f, cargs[11].f,
						 cargs[12].f, cargs[13].f, cargs[14].f, cargs[15].f);

	}
	EXIT("Unsupported " + name + " constructor.");
}

FunctionsLibrary::FunctionsLibrary(){
	_funcMap = {
		{ "clamp", { &FunctionsLibrary::funcClamp, {3}, "(x, min, max)" } },
		{ "pow", { &FunctionsLibrary::funcPow, {2}, "(x, a)" } },
		{ "min", { &FunctionsLibrary::funcMin, {2}, "(x, y)" } },
		{ "max", { &FunctionsLibrary::funcMax, {2}, "(x, y)" } },
		{ "saturate", { &FunctionsLibrary::funcSaturate, {1}, "(x, y)" } },
		{ "bin", { &FunctionsLibrary::funcBin, {1}, "(i)" } },
		{ "hex", { &FunctionsLibrary::funcHex, {1}, "(i)" } },
		{ "oct", { &FunctionsLibrary::funcOct, {1}, "(i)" } },
		{ "dec", { &FunctionsLibrary::funcDec, {1}, "(i)" } },
		{ "cos", { &FunctionsLibrary::funcCos, {1}, "(x)" } },
		{ "sin", { &FunctionsLibrary::funcSin, {1}, "(x)" } },
		{ "tan", { &FunctionsLibrary::funcTan, {1}, "(x)" } },
		{ "acos", { &FunctionsLibrary::funcAcos, {1}, "(x)" } },
		{ "asin", { &FunctionsLibrary::funcAsin, {1}, "(x)" } },
		{ "atan", { &FunctionsLibrary::funcAtan, {1, 2}, "(x), (y, x)" } },
		{ "atan2", { &FunctionsLibrary::funcAtan, {2}, "(y, x)" } },
		{ "exp", { &FunctionsLibrary::funcExp, {1}, "(x)" } },
		{ "log", { &FunctionsLibrary::funcLog, {1}, "(x)"} },
		{ "exp2", { &FunctionsLibrary::funcExp2, {1}, "(x)" } },
		{ "log2", { &FunctionsLibrary::funcLog2, {1} , "(x)"} },
		{ "sqrt", { &FunctionsLibrary::funcSqrt, {1}, "(x)" } },
		{ "xor", { &FunctionsLibrary::funcXor, {2}, "(i, j)" } },
		{ "floor", { &FunctionsLibrary::funcFloor, {1}, "(x)" } },
		{ "ceil", { &FunctionsLibrary::funcCeil, {1}, "(x)" } },
		{ "fract", { &FunctionsLibrary::funcFract, {1}, "(x)" } },
		{ "frac", { &FunctionsLibrary::funcFract, {1}, "(x)" } },
		{ "mix", { &FunctionsLibrary::funcMix, {3}, "(x, y, t)" } },
		{ "lerp", { &FunctionsLibrary::funcMix, {3}, "(x, y, t)" } },
		{ "abs", { &FunctionsLibrary::funcAbs, {1}, "(x)" } },
		{ "inversesqrt", { &FunctionsLibrary::funcInversesqrt, {1}, "(x)" } },
		{ "rcp", { &FunctionsLibrary::funcRcp, {1}, "(x)" } },
		{ "sign", { &FunctionsLibrary::funcSign, {1}, "(x)" } },
		{ "mod", { &FunctionsLibrary::funcMod, {2}, "(x, a)" } },
		{ "step", { &FunctionsLibrary::funcStep, {2}, "(e, x)" } },
		{ "smoothstep", { &FunctionsLibrary::funcSmoothstep, {3}, "(e0, e1, x)" } },
		{ "length", { &FunctionsLibrary::funcLength, {1}, "(x)" } },
		{ "distance", { &FunctionsLibrary::funcDistance, {2}, "(x, y)" } },
		{ "dot", { &FunctionsLibrary::funcDot, {2}, "(x, y)" } },
		{ "cross", { &FunctionsLibrary::funcCross, {2}, "(x, y)" } },
		{ "normalize", { &FunctionsLibrary::funcNormalize, {1}, "(x)" } },
		{ "reflect", { &FunctionsLibrary::funcReflect, {2}, "(i, n)" } },
		{ "refract", { &FunctionsLibrary::funcRefract, {3}, "(i, n, eta)" } },
		{ "inverse", { &FunctionsLibrary::funcInverse, {1}, "(x)" } },
		{ "transpose", { &FunctionsLibrary::funcTranspose, {1}, "(x)" } },
		{ "matrixCompMult", { &FunctionsLibrary::funcMatrixCompMult, {2}, "(x, y)" } },
		{ "hadamard", { &FunctionsLibrary::funcMatrixCompMult, {2}, "(x, y)" } },
		{ "schur", { &FunctionsLibrary::funcMatrixCompMult, {2}, "(x, y)" } },
		{ "radians", { &FunctionsLibrary::funcRadians, {1}, "(x)" } },
		{ "degrees", { &FunctionsLibrary::funcDegrees, {1}, "(x)" } },
		{ "sinh", { &FunctionsLibrary::funcSinh, {1}, "(x)" } },
		{ "cosh", { &FunctionsLibrary::funcCosh, {1}, "(x)" } },
		{ "tanh", { &FunctionsLibrary::funcTanh, {1}, "(x)" } },
		{ "asinh", { &FunctionsLibrary::funcAsinh, {1}, "(x)"} },
		{ "acosh", { &FunctionsLibrary::funcAcosh, {1}, "(x)"} },
		{ "atanh", { &FunctionsLibrary::funcAtanh, {1}, "(x)"} },
		{ "round", { &FunctionsLibrary::funcRound, {1}, "(x)"} },
		{ "trunc", { &FunctionsLibrary::funcTrunc, {1}, "(x)"} },
		{ "outerProduct", { &FunctionsLibrary::funcOuterProduct, {2}, "(x, y)" } },
		{ "determinant", { &FunctionsLibrary::funcDeterminant, {1}, "(x)" } },

		{ "lookAt", { &FunctionsLibrary::funcLookAt, {3}, "(eye, center, up)" } },
		{ "perspective", { &FunctionsLibrary::funcPerspective, {4}, "(fovy, aspect, near, far)" } },
		{ "ortho", { &FunctionsLibrary::funcOrthographic, {6}, "(left, right, bottom, top, near, far)"  } },
		{ "rotation", { &FunctionsLibrary::funcAxisRotationMat, {2}, "(angle, axis)"  } },
		{ "translation", { &FunctionsLibrary::funcTranslationMat, {1}, "(x)" } },
		{ "scale", { &FunctionsLibrary::funcScalingMat, {1}, "(x)" } },

		{ "vec3", { &FunctionsLibrary::constructorVec3, {1, 3}, "(v), (x, y, z)" } },
		{ "float3", { &FunctionsLibrary::constructorVec3, {1, 3}, "(v), (x, y, z)" } },
		{ "vec4", { &FunctionsLibrary::constructorVec4, {1, 2, 4}, "(v), (x, v), (v, x), (x, y, z, w)" } },
		{ "float4", { &FunctionsLibrary::constructorVec4, {1, 2, 4}, "(v), (x, v), (v, x), (x, y, z, w)" } },
		{ "mat3", { &FunctionsLibrary::constructorMat3, {1, 3, 9}, "(m), (cols...), (coeffs...)" } },
		{ "float3x3", { &FunctionsLibrary::constructorMat3, {1, 3, 9}, "(m), (cols...), (coeffs...)" } },
		{ "mat4", { &FunctionsLibrary::constructorMat4, {1, 4, 16}, "(m), (cols...), (coeffs...)" } },
		{ "float4x4", { &FunctionsLibrary::constructorMat4, {1, 4, 16} , "(m), (cols...), (coeffs...)"} },
	};
}

bool FunctionsLibrary::hasFunc(const std::string& name) const {
	return _funcMap.count(name) > 0;
}

bool FunctionsLibrary::validArgCount(const std::string& name, size_t argCount) const {
	const std::vector<size_t>& counts = _funcMap.at(name).allowedCounts;
	return std::find(counts.begin(), counts.end(), argCount) != counts.end();
}

Value FunctionsLibrary::eval(const std::string& name, const std::vector<Value>& args, ExpEval& evaluator) {
	return (this->*(_funcMap.at(name).call))(args, evaluator, name);
}

void FunctionsLibrary::populateDescriptions(std::unordered_map<std::string, std::string>& list) const {
	list.clear();
	list.reserve(_funcMap.size());
	for(const auto& func : _funcMap){
		list[func.first] = func.second.description;
	}
}
