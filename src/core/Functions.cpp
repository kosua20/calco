#include "core/Functions.hpp"

void Variables::setVar(const std::string& name, const Value& value){
	_variables[name] = value;
}

bool Variables::hasVar(const std::string& name) const {
	return _variables.count(name) != 0;
}

const Value& Variables::getVar(const std::string& name) const {
	return _variables.at(name);
}
