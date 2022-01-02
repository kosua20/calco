#pragma once
#include "core/Common.hpp"
#include "core/Functions.hpp"

class Calculator {
public:

	/// TODO: cleanup to avoid duplication in the app (add a few enum values? or a separate IN/OUT/ERROR?) 
	struct SemanticInfo {
		enum class Type {
			LITERAL, VARIABLE, FUNCTION, OPERATOR, SEPARATOR
		};

		Type type;
		long location;
		long size;
	};

	bool evaluate(const std::string& input, std::string& output, std::vector<SemanticInfo>& info);

private:

	Scope _globals;
	FunctionsLibrary _stdlib;

	unsigned long _funcCounter = 0;

};
