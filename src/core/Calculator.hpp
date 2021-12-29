#pragma once
#include "core/Common.hpp"
#include "core/Functions.hpp"

class Calculator {
public:
	bool evaluate(const std::string& input, std::string& output);

private:

	Scope _globals;

	unsigned long _funcCounter = 0;
	// User defined functions...
	// Settings
};
