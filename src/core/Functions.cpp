#include "core/Functions.hpp"
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

#define EXIT(msg) Log::Error() << (msg) << std::endl; return false;

bool allArgs(const std::vector<Value>& args, Value::Type type){
	for(const auto& arg : args){
		if(arg.type != type){
			return false;
		}
	}
	return true;
}

Value FunctionsLibrary::funcClamp(const std::vector<Value>& args){
	assert(args.size() == 3);
	const Value& x = args[0];
	Value a, b;
	if(!args[1].convert(x.type, a) || !args[2].convert(x.type, b)){
		EXIT("Unable to convert all arguments to same type.");
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcPow(const std::vector<Value>& args){
	assert(args.size() == 2);
	const Value& x = args[0];
	Value e;
	// TODO: maxtype instead.
	if(!args[1].convert(x.type, e)){
		EXIT("Unable to convert all arguments to same type.");
	}
	switch(x.type){
		case Value::BOOL:
			return glm::pow(x.b, e.b);
		case Value::INTEGER:
			return glm::min(x.i, e.i);
		case Value::FLOAT:
			return glm::min(x.f, e.f);
		case Value::VEC3:
			return glm::min(x.v3, e.v3);
		case Value::VEC4:
			return glm::min(x.v4, e.v4);
		default:
			break;
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcMin(const std::vector<Value>& args){
	assert(args.size() == 2);
	const Value::Type maxType = std::max(args[0].type, args[1].type);
	Value a, b;
	if(!args[0].convert(maxType, a) || !args[1].convert(maxType, b)){
		EXIT("Unable to convert all arguments to same type.");
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcMax(const std::vector<Value>& args){
	assert(args.size() == 2);
	const Value::Type maxType = std::max(args[0].type, args[1].type);
	Value a, b;
	if(!args[0].convert(maxType, a) || !args[1].convert(maxType, b)){
		EXIT("Unable to convert all arguments to same type.");
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcSaturate(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcBin(const std::vector<Value>& args){
	assert(args.size() == 1);
	if(args[0].type == Value::INTEGER){
		// TODO: modify output flag
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcHex(const std::vector<Value>& args){
	assert(args.size() == 1);
	if(args[0].type == Value::INTEGER){
		// TODO: modify output flag
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcOct(const std::vector<Value>& args){
	assert(args.size() == 1);
	if(args[0].type == Value::INTEGER){
		// TODO: modify output flag
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcDec(const std::vector<Value>& args){
	assert(args.size() == 1);
	if(args[0].type == Value::INTEGER){
		// TODO: modify output flag
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcCos(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcSin(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcTan(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcAcos(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcAsin(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcAtan(const std::vector<Value>& args){
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
			EXIT("Arguments have incompatible types.");
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

	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcExp(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcLog(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcExp2(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcLog2(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcSqrt(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcXor(const std::vector<Value>& args){
	assert(args.size() == 2);
	if(allArgs(args, Value::INTEGER)){
		return args[0].i ^ args[1].i;
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcFloor(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcCeil(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcFract(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcMix(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcAbs(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcInversesqrt(const std::vector<Value>& args){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::INTEGER:
			return glm::inversesqrt(args[0].i);
		case Value::FLOAT:
			return glm::inversesqrt(args[0].f);
		case Value::VEC3:
			return glm::inversesqrt(args[0].v3);
		case Value::VEC4:
			return glm::inversesqrt(args[0].v4);
		default:
			break;
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcRcp(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcSign(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcMod(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcStep(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcSmoothstep(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcLength(const std::vector<Value>& args){
	assert(args.size() == 1);
	if(args[0].type == Value::VEC3){
		return glm::length(args[0].v3);
	}
	if(args[0].type == Value::VEC4){
		return glm::length(args[0].v4);
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcDistance(const std::vector<Value>& args){
	assert(args.size() == 2);
	if(allArgs(args, Value::VEC3)){
		return glm::distance(args[0].v3, args[1].v3);
	}
	if(allArgs(args, Value::VEC4)){
		return glm::distance(args[0].v4, args[1].v4);
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcDot(const std::vector<Value>& args){
	assert(args.size() == 2);
	if(allArgs(args, Value::VEC3)){
		return glm::dot(args[0].v3, args[1].v3);
	}
	if(allArgs(args, Value::VEC4)){
		return glm::dot(args[0].v4, args[1].v4);
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcCross(const std::vector<Value>& args){
	assert(args.size() == 2);
	if(allArgs(args, Value::VEC3)){
		return glm::cross(args[0].v3, args[1].v3);
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcNormalize(const std::vector<Value>& args){
	assert(args.size() == 1);
	if(args[0].type == Value::VEC3){
		return glm::normalize(args[0].v3);
	}
	if(args[0].type == Value::VEC4){
		return glm::normalize(args[0].v4);
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcReflect(const std::vector<Value>& args){
	assert(args.size() == 2);
	if(allArgs(args, Value::VEC3)){
		return glm::reflect(args[0].v3, args[1].v3);
	}
	if(allArgs(args, Value::VEC4)){
		return glm::reflect(args[0].v4, args[1].v4);
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcRefract(const std::vector<Value>& args){
	assert(args.size() == 3);
	if(args[0].type == Value::VEC3 && args[1].type == Value::VEC3 && args[2].type == Value::FLOAT){
		return glm::refract(args[0].v3, args[1].v3, float(args[2].f));
	}
	if(args[0].type == Value::VEC4 && args[1].type == Value::VEC4 && args[2].type == Value::FLOAT){
		return glm::refract(args[0].v4, args[1].v4, float(args[2].f));
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcInverse(const std::vector<Value>& args){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::MAT3:
			return glm::inverse(args[0].m3);
		case Value::MAT4:
			return glm::inverse(args[0].m4);
		default:
			break;
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcTranspose(const std::vector<Value>& args){
	assert(args.size() == 1);
	switch(args[0].type){
		case Value::MAT3:
			return glm::transpose(args[0].m3);
		case Value::MAT4:
			return glm::transpose(args[0].m4);
		default:
			break;
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcMatrixCompMult(const std::vector<Value>& args){
	assert(args.size() == 2);
	if(allArgs(args, Value::MAT4)){
		return glm::matrixCompMult(args[0].m4, args[1].m4);
	}
	if(allArgs(args, Value::MAT3)){
		return glm::matrixCompMult(args[0].m3, args[1].m3);
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcRadians(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcDegrees(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcSinh(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcCosh(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcTanh(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcAsinh(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcAcosh(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}


Value FunctionsLibrary::funcAtanh(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcRound(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcTrunc(const std::vector<Value>& args){
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
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcOuterProduct(const std::vector<Value>& args){
	assert(args.size() == 2);
	if(allArgs(args, Value::VEC3)){
		return glm::outerProduct(args[0].v3, args[1].v3);
	}
	if(allArgs(args, Value::VEC4)){
		return glm::outerProduct(args[0].v4, args[1].v4);
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcDeterminant(const std::vector<Value>& args){
	assert(args.size() == 1);
	if(args[0].type == Value::MAT3){
		return glm::determinant(args[0].m3);
	}
	if(args[0].type == Value::MAT4){
		return glm::determinant(args[0].m4);
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcLookAt(const std::vector<Value>& args){
	assert(args.size() == 3);
	if(allArgs(args, Value::VEC3)){
		return glm::lookAt(args[0].v3, args[1].v3, args[2].v3);
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcPerspective(const std::vector<Value>& args){
	assert(args.size() == 4);
	if(allArgs(args, Value::FLOAT)){
		return glm::mat4(glm::perspective(args[0].f, args[1].f, args[2].f, args[3].f));
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcOrthographic(const std::vector<Value>& args){
	assert(args.size() == 6);
	if(allArgs(args, Value::FLOAT)){
		return glm::mat4(glm::ortho(args[0].f, args[1].f, args[2].f, args[3].f, args[4].f, args[5].f));
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcAxisRotationMat(const std::vector<Value>& args){
	assert(args.size() == 2);
	if(args[0].type == Value::FLOAT && args[1].type == Value::VEC3){
		return glm::rotate(glm::mat4(1.0f), float(args[0].f), args[1].v3);
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcTranslationMat(const std::vector<Value>& args){
	assert(args.size() == 1);
	if(args[0].type == Value::VEC3){
		return glm::translate(glm::mat4(1.0f), args[0].v3);
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::funcScalingMat(const std::vector<Value>& args){
	assert(args.size() == 1);
	if(args[0].type == Value::VEC3){
		return glm::scale(glm::mat4(1.0f), args[0].v3);
	} else if(args[0].type == Value::FLOAT){
		return glm::scale(glm::mat4(1.0f), glm::vec3(args[0].f));
	}
	EXIT("Unsupported type.");
}

Value FunctionsLibrary::constructorVec3(const std::vector<Value>& args){
	if(args.size() == 1){
		switch (args[0].type) {
			case Value::INTEGER:
				return glm::vec3(double(args[0].i));
			case Value::FLOAT:
				return glm::vec3(args[0].f);
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
				EXIT("Unable to convert argument.");
			}
		}
		return glm::vec3(cargs[0].f, cargs[1].f, cargs[2].f);
	}
	EXIT("Unsupported constructor.");
}

Value FunctionsLibrary::constructorVec4(const std::vector<Value>& args){
	if(args.size() == 1){
		switch (args[0].type) {
			case Value::INTEGER:
				return glm::vec4(double(args[0].i));
			case Value::FLOAT:
				return glm::vec4(args[0].f);
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
				EXIT("Unable to convert argument.");
			}
		}
		return glm::vec4(cargs[0].f, cargs[1].f, cargs[2].f, cargs[3].f);
	}
	EXIT("Unsupported constructor.");
}

Value FunctionsLibrary::constructorMat3(const std::vector<Value>& args){
	if(args.size() == 1){
		switch (args[0].type) {
			case Value::INTEGER:
				return glm::mat3(double(args[0].i));
			case Value::FLOAT:
				return glm::mat3(args[0].f);
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
				EXIT("Unable to convert argument.");
			}
		}
		return glm::mat3(cargs[0].v3, cargs[1].v3, cargs[2].v3);

	} else if(args.size() == 9){
		std::array<Value, 9> cargs;
		for(int i = 0; i < 9; ++i){
			if(!args[i].convert(Value::FLOAT, cargs[i])){
				EXIT("Unable to convert argument.");
			}
		}
		return glm::mat3(cargs[0].f, cargs[1].f, cargs[2].f,
						 cargs[3].f, cargs[4].f, cargs[5].f,
						 cargs[6].f, cargs[7].f, cargs[8].f);

	}
	EXIT("Unsupported constructor.");
}

Value FunctionsLibrary::constructorMat4(const std::vector<Value>& args){
	if(args.size() == 1){
		switch (args[0].type) {
			case Value::INTEGER:
				return glm::mat4(double(args[0].i));
			case Value::FLOAT:
				return glm::mat4(args[0].f);
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
				EXIT("Unable to convert argument.");
			}
		}
		return glm::mat4(cargs[0].v4, cargs[1].v4, cargs[2].v4, cargs[3].v4);

	} else if(args.size() == 16){
		std::array<Value, 16> cargs;
		for(int i = 0; i < 16; ++i){
			if(!args[i].convert(Value::FLOAT, cargs[i])){
				EXIT("Unable to convert argument.");
			}
		}
		return glm::mat4(cargs[0].f, cargs[1].f, cargs[2].f, cargs[3].f,
						 cargs[4].f, cargs[5].f, cargs[6].f, cargs[7].f,
						 cargs[8].f, cargs[9].f, cargs[10].f, cargs[11].f,
						 cargs[12].f, cargs[13].f, cargs[14].f, cargs[15].f);

	}
	EXIT("Unsupported constructor.");
}


FunctionsLibrary::FunctionsLibrary(){
	_funcMap = {
		{ "clamp", { &FunctionsLibrary::funcClamp, {3} } },
		{ "pow", { &FunctionsLibrary::funcPow, {2} } },
		{ "min", { &FunctionsLibrary::funcMin, {2} } },
		{ "max", { &FunctionsLibrary::funcMax, {2} } },
		{ "saturate", { &FunctionsLibrary::funcSaturate, {1} } },
		{ "bin", { &FunctionsLibrary::funcBin, {1} } },
		{ "hex", { &FunctionsLibrary::funcHex, {1} } },
		{ "oct", { &FunctionsLibrary::funcOct, {1} } },
		{ "dec", { &FunctionsLibrary::funcDec, {1} } },
		{ "cos", { &FunctionsLibrary::funcCos, {1} } },
		{ "sin", { &FunctionsLibrary::funcSin, {1} } },
		{ "tan", { &FunctionsLibrary::funcTan, {1} } },
		{ "acos", { &FunctionsLibrary::funcAcos, {1} } },
		{ "asin", { &FunctionsLibrary::funcAsin, {1} } },
		{ "atan", { &FunctionsLibrary::funcAtan, {1, 2} } },
		{ "atan2", { &FunctionsLibrary::funcAtan, {2} } },
		{ "exp", { &FunctionsLibrary::funcExp, {1} } },
		{ "log", { &FunctionsLibrary::funcLog, {1} } },
		{ "exp2", { &FunctionsLibrary::funcExp2, {1} } },
		{ "log2", { &FunctionsLibrary::funcLog2, {1} } },
		{ "sqrt", { &FunctionsLibrary::funcSqrt, {1} } },
		{ "xor", { &FunctionsLibrary::funcXor, {2} } },
		{ "floor", { &FunctionsLibrary::funcFloor, {1} } },
		{ "ceil", { &FunctionsLibrary::funcCeil, {1} } },
		{ "fract", { &FunctionsLibrary::funcFract, {1} } },
		{ "frac", { &FunctionsLibrary::funcFract, {1} } },
		{ "mix", { &FunctionsLibrary::funcMix, {3} } },
		{ "lerp", { &FunctionsLibrary::funcMix, {3} } },
		{ "abs", { &FunctionsLibrary::funcAbs, {1} } },
		{ "inversesqrt", { &FunctionsLibrary::funcInversesqrt, {1} } },
		{ "rcp", { &FunctionsLibrary::funcRcp, {1} } },
		{ "sign", { &FunctionsLibrary::funcSign, {1} } },
		{ "mod", { &FunctionsLibrary::funcMod, {2} } },
		{ "step", { &FunctionsLibrary::funcStep, {2} } },
		{ "smoothstep", { &FunctionsLibrary::funcSmoothstep, {3} } },
		{ "length", { &FunctionsLibrary::funcLength, {1} } },
		{ "distance", { &FunctionsLibrary::funcDistance, {2} } },
		{ "dot", { &FunctionsLibrary::funcDot, {2} } },
		{ "cross", { &FunctionsLibrary::funcCross, {2} } },
		{ "normalize", { &FunctionsLibrary::funcNormalize, {1} } },
		{ "reflect", { &FunctionsLibrary::funcReflect, {2} } },
		{ "refract", { &FunctionsLibrary::funcRefract, {3} } },
		{ "inverse", { &FunctionsLibrary::funcInverse, {1} } },
		{ "transpose", { &FunctionsLibrary::funcTranspose, {1} } },
		{ "matrixCompMult", { &FunctionsLibrary::funcMatrixCompMult, {2} } },
		{ "hadamard", { &FunctionsLibrary::funcMatrixCompMult, {2} } },
		{ "schur", { &FunctionsLibrary::funcMatrixCompMult, {2} } },
		{ "radians", { &FunctionsLibrary::funcRadians, {1} } },
		{ "degrees", { &FunctionsLibrary::funcDegrees, {1} } },
		{ "sinh", { &FunctionsLibrary::funcSinh, {1} } },
		{ "cosh", { &FunctionsLibrary::funcCosh, {1} } },
		{ "tanh", { &FunctionsLibrary::funcTanh, {1} } },
		{ "asinh", { &FunctionsLibrary::funcAsinh, {1} } },
		{ "acosh", { &FunctionsLibrary::funcAcosh, {1} } },
		{ "atanh", { &FunctionsLibrary::funcAtanh, {1} } },
		{ "round", { &FunctionsLibrary::funcRound, {1} } },
		{ "trunc", { &FunctionsLibrary::funcTrunc, {1} } },
		{ "outerProduct", { &FunctionsLibrary::funcOuterProduct, {2} } },
		{ "determinant", { &FunctionsLibrary::funcDeterminant, {1} } },

		{ "lookAt", { &FunctionsLibrary::funcLookAt, {3} } },
		{ "perspective", { &FunctionsLibrary::funcPerspective, {4} } },
		{ "ortho", { &FunctionsLibrary::funcOrthographic, {6} } },
		{ "rotation", { &FunctionsLibrary::funcAxisRotationMat, {2} } },
		{ "translation", { &FunctionsLibrary::funcTranslationMat, {1} } },
		{ "scale", { &FunctionsLibrary::funcScalingMat, {1} } },

		{ "vec3", { &FunctionsLibrary::constructorVec3, {1, 3} } },
		{ "float3", { &FunctionsLibrary::constructorVec3, {1, 3} } },
		{ "vec4", { &FunctionsLibrary::constructorVec4, {1, 2, 4} } },
		{ "float4", { &FunctionsLibrary::constructorVec4, {1, 2, 4} } },
		{ "mat3", { &FunctionsLibrary::constructorMat3, {1, 3, 9} } },
		{ "float3x3", { &FunctionsLibrary::constructorMat3, {1, 3, 9} } },
		{ "mat4", { &FunctionsLibrary::constructorMat4, {1, 4, 16} } },
		{ "float4x4", { &FunctionsLibrary::constructorMat4, {1, 4, 16} } },
	};
}

bool FunctionsLibrary::hasFunc(const std::string& name) const {
	return _funcMap.count(name) > 0;
}

bool FunctionsLibrary::validArgCount(const std::string& name, size_t argCount) const {
	const std::vector<size_t>& counts = _funcMap.at(name).allowedCounts;
	return std::find(counts.begin(), counts.end(), argCount) != counts.end();
}

Value FunctionsLibrary::eval(const std::string& name, const std::vector<Value>& args) {
	
	return (this->*(_funcMap.at(name).call))(args);
}
