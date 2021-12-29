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
