#pragma once
#include "core/Common.hpp"
#include "core/Types.hpp"
#include <unordered_map>

class ExpEval;

class Scope {
public:

	using VariableList = std::unordered_map<std::string, Value>;
	using FunctionList = std::unordered_map<std::string, std::shared_ptr<FunctionDef>>;

	void setVar(const std::string& name, const Value& value);

	bool hasVar(const std::string& name) const;

	const Value& getVar(const std::string& name) const;

	void setFunc(const std::string& name, const std::shared_ptr<FunctionDef>& func);

	bool hasFunc(const std::string& name) const;

	const std::shared_ptr<FunctionDef>& getFunc(const std::string& name) const;

	const VariableList& getVars() const { return _variables; }

	const FunctionList& getFuncs() const { return _functions; }

private:

	VariableList _variables;
	FunctionList _functions;

};

class FunctionsLibrary {
public:

	FunctionsLibrary();

	bool hasFunc(const std::string& name) const ;
	bool validArgCount(const std::string& name, size_t argCount) const;

	Value eval(const std::string& name, const std::vector<Value>& args, ExpEval& evaluator);

	void populateDescriptions(std::unordered_map<std::string, std::string>& list) const;

private:
	
	Value funcClamp(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcPow(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcMin(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcMax(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcSaturate(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcBin(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcHex(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcOct(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcDec(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcCos(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcSin(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcTan(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcAcos(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcAsin(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcAtan(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcExp(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcLog(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcExp2(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcLog2(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcSqrt(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcXor(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcFloor(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcCeil(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcFract(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcMix(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcAbs(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcInversesqrt(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcRcp(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcSign(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcMod(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcStep(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcSmoothstep(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcLength(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcDistance(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcDot(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcCross(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcNormalize(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcReflect(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcRefract(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcInverse(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcTranspose(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcMatrixCompMult(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcRadians(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcDegrees(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcSinh(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcCosh(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcTanh(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcAsinh(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcAcosh(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcAtanh(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcRound(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcTrunc(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcOuterProduct(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcDeterminant(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);

	Value funcLookAt(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcPerspective(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcOrthographic(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcAxisRotationMat(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcTranslationMat(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value funcScalingMat(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);

	Value constructorVec3(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value constructorVec4(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value constructorMat3(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);
	Value constructorMat4(const std::vector<Value>& args, ExpEval& evaluator, const std::string& name);

	struct FunctionInfos {
		Value (FunctionsLibrary::*call)(const std::vector<Value>&, ExpEval& evaluator, const std::string&);
		std::vector<size_t> allowedCounts;
		std::string description;
	};

	std::unordered_map<std::string, FunctionInfos> _funcMap;
};
