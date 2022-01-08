#pragma once
#include "core/Common.hpp"
#include "core/Functions.hpp"
#include <map>

class Documentation {
public:

	struct Function {
		std::string name;
		std::string expression;
	};

	struct Variable {
		std::string value;
		uint count;
	};

	// We want lexicographic ordering for a nicer listing display.
	using Functions = std::map<std::string, Function>;
	using Variables = std::map<std::string, Variable>;

	void setVar(const std::string& name, const Value& value);
	void setFunc(const std::string& name, const std::shared_ptr<FunctionDef>& def);
	void setLibrary(const FunctionsLibrary& library);

	void setFormat(Format format){ _format = format; }

	const Functions& functions() const { return _functions; }
	const Functions& stdlib() const { return _stdlib; }
	const Variables& variables() const { return _variables; }
	Format format() const { return _format; }
	
	void clear();
	
private:

	Functions _functions;
	Functions _stdlib;
	Variables _variables;

	Format _format = Format::BASE_10_FLAG;
};

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

	Calculator();

	bool evaluate(const std::string& input, Value& output, std::vector<Word>& info, Format& format);

	void clear();

	void updateDocumentation(Format format);

	void saveToStream(std::ostream& str) const;
	
	void loadFromStream(std::istream& str);
	
	const Documentation::Functions& functions() const { return _doc.functions(); }
	const Documentation::Functions& stdlib() const { return _doc.stdlib(); }
	const Documentation::Variables& variables() const { return _doc.variables(); }

private:

	Scope _globals;
	FunctionsLibrary _stdlib;
	Documentation _doc;

	unsigned long _funcCounter = 0;

};
