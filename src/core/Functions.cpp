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
