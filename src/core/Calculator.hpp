#pragma once
#include "core/Common.hpp"
#include "core/Functions.hpp"

class Calculator {
public:

	struct Word {
		enum Type {
			LITERAL = 0, VARIABLE, FUNCTION, OPERATOR, SEPARATOR, RESULT, COUNT
		};

		Type type;
		long location;
		long size;
	};

	bool evaluate(const std::string& input, Value& output, std::vector<Word>& info, Format& format);

	void clear();
	
	void saveToStream(std::ostream& str) const;
	void loadFromStream(std::istream& str);

	const std::unordered_map<std::string, std::pair<std::string, std::string>>& functions() const { return _functions; }
	const std::unordered_map<std::string, std::string>& variables() const { return _variables; }

private:

	Scope _globals;
	FunctionsLibrary _stdlib;

	/// TODO: maybe move in scope ? but only valid for global scope
	std::unordered_map<std::string, std::pair<std::string, std::string>> _functions;
	std::unordered_map<std::string, std::string> _variables;

	unsigned long _funcCounter = 0;

};
