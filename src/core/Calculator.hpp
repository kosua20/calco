#pragma once
#include "core/Common.hpp"
#include "core/Functions.hpp"

class Calculator {
public:

	struct Word {
		enum Type {
			LITERAL = 0, VARIABLE, FUNCTION, OPERATOR, SEPARATOR, COUNT
		};

		Type type;
		long location;
		long size;
	};

	bool evaluate(const std::string& input, Value& output, std::vector<Word>& info, Format& format);

private:

	Scope _globals;
	FunctionsLibrary _stdlib;

	unsigned long _funcCounter = 0;

};
