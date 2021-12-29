#include "core/Scanner.hpp"
#include "core/system/TextUtilities.hpp"

Scanner::Scanner(const std::string& input) {
	_input = input;
	_inputSize = long(_input.size());
}

char Scanner::at(long pos) const{
	return _input.at(pos);
}

bool Scanner::valid(long pos) const{
	return pos < _inputSize;
}

bool Scanner::isBinDigitAt(long pos) const {
	if(!valid(pos)){
		return false;
	}
	const char a = at(pos);
	return a == '0' || a == '1';
}

bool Scanner::isOctaDigitAt(long pos) const {
	if(!valid(pos)){
		return false;
	}
	const char a = at(pos);
	return (a >= '0' && a <= '7');
}

bool Scanner::isDeciDigitAt(long pos) const {
	if(!valid(pos)){
		return false;
	}
	const char a = at(pos);
	return (a >= '0' && a <= '9');
}

bool Scanner::isHexaDigitAt(long pos) const {
	if(!valid(pos)){
		return false;
	}
	const char a = at(pos);
	return (a >= '0' && a <= '9') || (a >= 'a' && a <= 'f') || (a >= 'A' && a <= 'F');
}

bool Scanner::isIdentifierCharAt(long pos) const {
	if(!valid(pos)){
		return false;
	}
	const char a = at(pos);
	return (a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || (a == '_');
}

Status Scanner::scan(){
	_tokens.clear();
	long position = 0;
	long firstErrorPosition = -1;

	if(_input.empty()){
		return Status(true);
	}

	while(valid(position)){
		const char c0 = at(position);
		// Skip whitespace.
		if(std::isspace(c0) || c0 == ';'){
			++position;
			continue;
		}
		const char c1 = valid(position + 1) ? at(position + 1) : '\0';
		const int startPosition = position;

		// All numbers, whatever their base, start with a digit or a dot followed by a digit.
		if((c0 >= '0' && c0 <= '9') || (c0 == '.' && (c1 >= '0' && c1 <= '9'))){
			int base = 10;
			bool isFloat = false;

			// Leading 0 == non decimal base.
			if(c0 == '0' && c1 != '.'){
				if(c1 == 'x' || c1 == 'X'){
					// Hexa
					base = 16;
					position += 2;
					// Read hexa
					while(isHexaDigitAt(position)){
						++position;
					}
				} else if(c1 == 'b' || c1 == 'B' ){
					// Binary
					base = 2;
					position += 2;
					// Read bin
					while(isBinDigitAt(position)){
						++position;
					}
				} else {
					// Octal, same trap as in C
					base = 8;
					position += 1;
					// Read octal
					while(isOctaDigitAt(position)){
						++position;
					}
				}
			} else {
				// Decimal, maybe a float.
				while(isDeciDigitAt(position)){
					++position;
				}
				// Fractional part.
				if(valid(position) && at(position) == '.'){
					isFloat = true;
					++position;
					while(isDeciDigitAt(position)){
						++position;
					}
				}
				// Exponential part.
				if(valid(position) && (at(position) == 'e' || at(position) == 'E')){
					isFloat = true;
					++position;
					if(valid(position) && (at(position) == '+' || at(position) == '-')){
						++position;
					}
					while(isDeciDigitAt(position)){
						++position;
					}
				}
			}
			// Read the number.
			const size_t tokenSize = position - startPosition;
			const std::string number = _input.substr(startPosition, tokenSize);


			try {
				if(isFloat){
					const float val = std::stod(number);
					// Only emplace once the conversion has succeeded.
					_tokens.emplace_back();
					_tokens.back().type = Token::Type::Float;
					_tokens.back().fVal = val;
					_tokens.back().dbgStartPos = startPosition;
					_tokens.back().dbgEndPos = position;
				} else {
					const long long val = std::stoll(number, nullptr, base);
					// Only emplace once the conversion has succeeded.
					_tokens.emplace_back();
					_tokens.back().type = Token::Type::Integer;
					_tokens.back().iVal = val;
					_tokens.back().dbgStartPos = startPosition;
					_tokens.back().dbgEndPos = position;
				}
			} catch(...) {
				firstErrorPosition = startPosition;
			}
			continue;
		}

		// Comment until the end of the line
		if(c0 == '/' && c1 == '/'){
			// Don't store any token for comments.
			position = _inputSize;
			continue;
		}

		// Identifier
		if(isIdentifierCharAt(position)){
			++position;
			while(isIdentifierCharAt(position) || isDeciDigitAt(position)){
				++position;
			}
			const std::string identifier = _input.substr(startPosition, position - startPosition);
			const std::string lowIdentifier = TextUtilities::lowercase(identifier);

			_tokens.emplace_back();
			_tokens.back().dbgStartPos = startPosition;
			_tokens.back().dbgEndPos = position;

			// Special case: constant name.
			if(MathConstants.count(lowIdentifier) != 0){
				_tokens.back().type = Token::Type::Float;
				_tokens.back().fVal = MathConstants.at(lowIdentifier);
				continue;
			}
			// Else this is an identifier
			_tokens.back().type = Token::Type::Identifier;
			_tokens.back().sVal = identifier;
			continue;
		}

		// Operator
		bool wasOperator = false;
		Operator opType;
		int opSize = 1;

		if(c0 == '('){
			wasOperator = true;
			opType = Operator::OpenParenth;

		} else if(c0 == ')'){
			wasOperator = true;
			opType = Operator::CloseParenth;

		} else if(c0 == '+'){
			wasOperator = true;
			opType = Operator::Plus;

		} else if(c0 == '-'){
			wasOperator = true;
			opType = Operator::Minus;

		} else if(c0 == '*'){
			wasOperator = true;
			opType = Operator::Product;

		} else if(c0 == '/'){
			wasOperator = true;
			opType = Operator::Divide;

		} else if(c0 == '^'){
			wasOperator = true;
			opType = Operator::Power;

		} else if(c0 == '%'){
			wasOperator = true;
			opType = Operator::Modulo;

		} else if(c0 == '='){
			wasOperator = true;

			if(c1 == '='){
				opType = Operator::Equal;
				opSize = 2;
			} else {
				opType = Operator::Assign;
			}

		} else if (c0 == '<'){
			wasOperator = true;

			if(c1 == '<'){
				opType = Operator::ShiftLeft;
				opSize = 2;
			} else if (c1 == '='){
				opType = Operator::LessThanEqual;
				opSize = 2;
			} else {
				opType = Operator::LessThan;
			}

		} else if (c0 == '>'){
			wasOperator = true;

			if(c1 == '>'){
				opType = Operator::ShiftRight;
				opSize = 2;
			} else if (c1 == '='){
				opType = Operator::GreaterThanEqual;
				opSize = 2;
			} else {
				opType = Operator::GreaterThan;
			}

		} else if (c0 == '|'){
			wasOperator = true;

			if(c1 == '|'){
				opType = Operator::BoolOr;
				opSize = 2;
			} else {
				opType = Operator::BitOr;
			}

		} else if (c0 == '&'){
			wasOperator = true;

			if(c1 == '&'){
				opType = Operator::BoolAnd;
				opSize = 2;
			} else {
				opType = Operator::BitAnd;
			}

		} else if (c0 == '~'){
			wasOperator = true;
			opType = Operator::BitNot;

		} else if (c0 == '@'){
			wasOperator = true;

			if(c1 == '@'){
				opType = Operator::BoolXor;
				opSize = 2;
			} else {
				opType = Operator::BitXor;
			}

		} else if (c0 == '!'){
			wasOperator = true;

			if(c1 == '='){
				opType = Operator::Different;
				opSize = 2;
			} else {
				opType = Operator::BoolNot;
			}

		} else if (c0 == '?'){
			wasOperator = true;
			opType = Operator::QuestionMark;

		} else if (c0 == ':'){
			wasOperator = true;
			opType = Operator::Colon;

		} else if (c0 == ','){
			wasOperator = true;
			opType = Operator::Comma;

		} else if (c0 == '.'){
			wasOperator = true;
			opType = Operator::Dot;

		}

		if(wasOperator){
			_tokens.emplace_back();
			_tokens.back().type = Token::Type::Operator;
			_tokens.back().opVal = opType;
			_tokens.back().dbgStartPos = startPosition;
			_tokens.back().dbgEndPos = startPosition + opSize - 1;
			position += opSize;
			continue;
		}

		// Error.
		firstErrorPosition = position;
		++position;
	}

	if(firstErrorPosition >= 0){
		return Status(firstErrorPosition, "Syntax");
	}

	return Status(true);
}
