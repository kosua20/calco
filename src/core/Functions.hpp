#pragma once
#include "core/Common.hpp"
#include "core/Types.hpp"
#include <unordered_map>

class Variables {
public:

	void setVar(const std::string& name, const Value& value);

	bool hasVar(const std::string& name) const;

	const Value& getVar(const std::string& name) const;

private:

	std::unordered_map<std::string, Value> _variables;

};
