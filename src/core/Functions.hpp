#pragma once
#include "core/Common.hpp"
#include "core/Types.hpp"
#include <unordered_map>

class Scope {
public:

	void setVar(const std::string& name, const Value& value);

	bool hasVar(const std::string& name) const;

	const Value& getVar(const std::string& name) const;

	void setFunc(const std::string& name, const std::shared_ptr<FunctionDef>& func);

	bool hasFunc(const std::string& name) const;

	const std::shared_ptr<FunctionDef>& getFunc(const std::string& name) const;

private:

	std::unordered_map<std::string, Value> _variables;

	std::unordered_map<std::string, std::shared_ptr<FunctionDef>> _functions;

};

class FunctionsLibrary {
public:

	FunctionsLibrary();

	bool hasFunc(const std::string& name) const ;
	bool validArgCount(const std::string& name, size_t argCount) const;

	Value eval(const std::string& name, const std::vector<Value>& args);

private:
	
	Value funcClamp(const std::vector<Value>& args);
	Value funcPow(const std::vector<Value>& args);
	Value funcMin(const std::vector<Value>& args);
	Value funcMax(const std::vector<Value>& args);
	Value funcSaturate(const std::vector<Value>& args);
	Value funcVec4();
	Value funcMat4();
	Value funcBin(const std::vector<Value>& args);
	Value funcHex(const std::vector<Value>& args);
	Value funcOct(const std::vector<Value>& args);
	Value funcDec(const std::vector<Value>& args);
	Value funcCos(const std::vector<Value>& args);
	Value funcSin(const std::vector<Value>& args);
	Value funcTan(const std::vector<Value>& args);
	Value funcAcos(const std::vector<Value>& args);
	Value funcAsin(const std::vector<Value>& args);
	Value funcAtan(const std::vector<Value>& args);
	Value funcExp(const std::vector<Value>& args);
	Value funcLog(const std::vector<Value>& args);
	Value funcExp2(const std::vector<Value>& args);
	Value funcLog2(const std::vector<Value>& args);
	Value funcSqrt(const std::vector<Value>& args);
	Value funcXor(const std::vector<Value>& args);
	Value funcFloor(const std::vector<Value>& args);
	Value funcCeil(const std::vector<Value>& args);
	Value funcFract(const std::vector<Value>& args);
	Value funcMix(const std::vector<Value>& args);
	Value funcAbs(const std::vector<Value>& args);
	Value funcInversesqrt(const std::vector<Value>& args);
	Value funcRcp(const std::vector<Value>& args);
	Value funcSign(const std::vector<Value>& args);
	Value funcMod(const std::vector<Value>& args);
	Value funcStep(const std::vector<Value>& args);
	Value funcSmoothstep(const std::vector<Value>& args);
	Value funcLength(const std::vector<Value>& args);
	Value funcDistance(const std::vector<Value>& args);
	Value funcDot(const std::vector<Value>& args);
	Value funcCross(const std::vector<Value>& args);
	Value funcNormalize(const std::vector<Value>& args);
	Value funcReflect(const std::vector<Value>& args);
	Value funcRefract(const std::vector<Value>& args);
	Value funcInverse(const std::vector<Value>& args);
	Value funcTranspose(const std::vector<Value>& args);
	Value funcMatrixCompMult(const std::vector<Value>& args);
	Value funcRadians(const std::vector<Value>& args);
	Value funcDegrees(const std::vector<Value>& args);
	Value funcSinh(const std::vector<Value>& args);
	Value funcCosh(const std::vector<Value>& args);
	Value funcTanh(const std::vector<Value>& args);
	Value funcAsinh(const std::vector<Value>& args);
	Value funcAcosh(const std::vector<Value>& args);
	Value funcAtanh(const std::vector<Value>& args);
	Value funcRound(const std::vector<Value>& args);
	Value funcTrunc(const std::vector<Value>& args);
	Value funcOuterProduct(const std::vector<Value>& args);
	Value funcDeterminant(const std::vector<Value>& args);
	Value constructorVec4(const std::vector<Value>& args);
	Value constructorMat4(const std::vector<Value>& args);

	struct FunctionInfos {
		Value (FunctionsLibrary::*call)(const std::vector<Value>&);
		std::vector<size_t> allowedCounts;
	};

	std::unordered_map<std::string, FunctionInfos> _funcMap;
};
