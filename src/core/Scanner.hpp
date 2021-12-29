#pragma once
#include "core/Common.hpp"
#include "core/Types.hpp"
#include "core/Functions.hpp"


struct Token {

	enum class Type {
		Float, Integer, Identifier, Operator
	};

	Type type = Type::Identifier;

	// No need for a union.
	double fVal = 0.0;
	long long iVal = 0;
	std::string sVal = "Unknown";
	Operator opVal = Operator::Assign;

	long dbgStartPos;
	long dbgEndPos;
};


class Scanner {
public:

	Scanner(const std::string& input);

	Status scan();

	const std::vector<Token>& tokens() const {
		return _tokens;
	}

private:

	char at(long pos) const;

	bool valid(long pos) const;

	bool isBinDigitAt(long pos) const;

	bool isOctaDigitAt(long pos) const;

	bool isDeciDigitAt(long pos) const;

	bool isHexaDigitAt(long pos) const;

	bool isIdentifierCharAt(long pos) const;

	std::string _input;
	long _inputSize;
	std::vector<Token> _tokens;
};
