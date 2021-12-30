#include "core/Functions.hpp"

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

/* list of functions
 clamp
 pow
 min
 max
 saturate
 vec4/float4
 mat4/float4x4
 bin
 hex
 oct
 dec
 cos
 sin
 tan
 acos
 asin
 atan
 atan2
 exp
 log
 exp2
 log2
 sqrt
 xor
 floor
 ceil
 frac/fract
 mix/lerp
 abs
 inversesqrt
 rcp
 sign
 mod
 step
 smoothstep
 length
 distance
 dot
 cross
 normalize
 reflect
 refract
 inverse
 transpose
 matrixCompMult/hadamard/schur
 radians
 degrees
 sinh
 cosh
 tanh
 asinh
 acosh
 atanh
 round
 trunc
 floatBitsToInt
 floatBitsToUint
 outerProduct
 determinant



 */

Value clamp(const std::vector<Value>& args){
	if(args.size() != 3){
		return false;
	}
	const Value::Type finalType = std::max(std::max(Value::INTEGER, args[0].type), std::max(args[1].type, args[2].type));
	Value conv0, conv1, conv2;
	// TODO: 'convert all to biggest type' helper
	args[0].convert(finalType, conv0);
	args[1].convert(finalType, conv1);
	args[2].convert(finalType, conv2);
	switch (finalType) {
		case Value::INTEGER:
			return glm::clamp(conv0.i, conv1.i, conv2.i);
			break;
		case Value::FLOAT:
			return glm::clamp(conv0.f, conv1.f, conv2.f);
			break;
		case Value::VEC4:
			return glm::clamp(conv0.vec, conv1.vec, conv2.vec);
			break;
		default:
			break;
	}
	return {};
}

Value vec4Constructor(const std::vector<Value>& args){
	if(args.size() == 1){
		switch (args[0].type) {
			case Value::VEC4:
				return args[0].vec;

			case Value::FLOAT:
			case Value::INTEGER:
			{
				Value conv;
				if(args[0].convert(Value::FLOAT, conv)){
					return glm::vec4(conv.f);
				}
				break;
			}
			default:
				break;
		}
	} else if(args.size() == 4){
		std::vector<Value> convArgs(4);
		bool succ = true;
		for(int i = 0; i < 4; ++i){
			succ = args[i].convert(Value::FLOAT, convArgs[i]) && succ;
		}
		if(succ){
			return glm::vec4(convArgs[0].f, convArgs[1].f, convArgs[2].f, convArgs[3].f);
		}
	}
	return {};
}

Value mat4Constructor(const std::vector<Value>& args){
	if(args.size() == 1){
		switch (args[0].type) {
			case Value::MAT4:
				return args[0].mat;
			case Value::FLOAT:
			case Value::INTEGER:
			{
				Value conv;
				if(args[0].convert(Value::FLOAT, conv)){
					return glm::mat4(conv.f);
				}
				break;
			}
			default:
				break;
		}
	} else if(args.size() == 4){
		std::vector<Value> convArgs(4);
		bool succ = true;
		for(int i = 0; i < 4; ++i){
			succ = args[i].convert(Value::VEC4, convArgs[i]) && succ;
		}
		if(succ){
			return glm::mat4(convArgs[0].vec, convArgs[1].vec, convArgs[2].vec, convArgs[3].vec);
		}
	} else if(args.size() == 16){
		std::vector<Value> convArgs(16);
		bool succ = true;
		for(int i = 0; i < 16; ++i){
			succ = args[i].convert(Value::FLOAT, convArgs[i]) && succ;
		}
		if(succ){
			glm::mat4 res;
			for(int i = 0; i < 4; ++i){
				for(int j = 0; j < 4; ++j){
					res[i][j] = convArgs[4*i+j].f;
				}
			}
			return res;
		}
	}
	return {};
}

static const std::unordered_map<std::string, Value (*)(const std::vector<Value>&)> funcMap = {
	{"vec4", &vec4Constructor},
	{"mat4", &mat4Constructor},
	{"clamp", &clamp},
};

bool FunctionsLibrary::hasFunc(const std::string& name) const {
	return funcMap.count(name) > 0;
}

Value FunctionsLibrary::eval(const FunctionCall& func, const std::vector<Value>& args) const {
	return (*(funcMap.at(func.name)))(args);
}
