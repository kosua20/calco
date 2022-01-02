#include "core/Parser.hpp"

/*
// Precedences
	18 "(", ")"
	17 "."
	16 "^"
	15 "~", "!", unary "+", unary "-"
	14 "*", "/", "%"
	13 binary "+", binary "-"
	12 "<<", ">>"
	11 "<", ">", "<=", ">="
	10 "==", "!="
	9 "&"
	8 "@"
	7 "|"
	6 "&&"
	4 "||"
	3 "?", ":"
	2 "="
	1 "," (special case for us)

 statement		→ declaration "=" expression | expression
 declaration	→ identifier | function
 function		→ identifier "(" ( identifier ( "," identifier )+ ) ")"
 expression     → ternary
 ternary		→ boolOr ("?" boolOr ":" boolOr)*
 boolOr			→ boolXor ( "||" boolXor)*
 boolXor		→ boolAnd ( "@@" boolAnd)*
 boolAnd		→ bitOr ( "&&" bitOr)*
 bitOr			→ bitXor ( "|" bitXor)*
 bitXor			→ bitAnd ( "@" bitAnd)*
 bitAnd			→ equality ( "&" equality)*
 equality       → comparison ( ( "!=" | "==" ) comparison )*
 comparison     → bitShift ( ( ">" | ">=" | "<" | "<=" ) bitShift )*
 bitShift     	→ term ( ( ">>" | "<<" ) term )*
 term           → factor ( ( "-" | "+" ) factor )* ;
 factor         → unary ( ( "/" | "*" | "%" ) unary )* ;
 unary          → ( "!" | "-" | "+" | "~" ) unary | power ;
 power			→ member ( "^" member)*
 member			→ primary ("." primary)*
 primary        → literal | identifier | "(" expression ")" | function call;
 */

#define EXIT_IF_FAILED(a) if(a == nullptr){ _failedToken = _failed ? _failedToken : _position; _failed = true; return nullptr;}
#define EXIT(message) if(true){ if(!_failed){ _failedToken = _position; _failedMessage = message; _failed = true; };  return nullptr; }

Parser::Parser(const std::vector<Token>& tokens) : _tokens(tokens), _tree(nullptr), _position(0), _tokenCount(tokens.size()) {

}

bool Parser::valid() const {
	return _position < _tokenCount;
}

bool Parser::expect(Operator op) const {
	if(!valid()){
		return false;
	}

	const Token& current = peek();
	if(current.type != Token::Type::Operator){
		return false;
	}
	if(current.opVal == op){
		return true;
	}
	return false;
}

bool Parser::match(Operator op) {
	if(expect(op)){
		advance();
		return true;
	}
	return false;
}

bool Parser::match(std::initializer_list<Operator> ops) {
	if(!valid()){
		return false;
	}

	const Token& current = peek();
	if(current.type != Token::Type::Operator){
		return false;
	}
	for(const Operator& op : ops){
		if(current.opVal == op){
			advance();
			return true;
		}
	}
	return false;
}

void Parser::advance(){
	++_position;
}

const Token& Parser::peek() const {
	return _tokens[_position];
};

const Token& Parser::previous() const {
	return _tokens[_position-1];
};

Operator Parser::previousOp() const {
	assert(previous().type == Token::Type::Operator);
	return previous().opVal;
};

Parser::Result Parser::statement(){
	// Because we have such a simple grammar, just look ahead for an "=" sign.
	bool assignment = false;
	for(const Token& token : _tokens){
		if(token.type == Token::Type::Operator && token.opVal == Operator::Assign){
			assignment = true;
			break;
		}
	}

	if(assignment){
		if(!valid()){
			EXIT("Missing variable or function name");
		}

		// Expect an identifier
		const Token& current = _tokens[_position];
		if(current.type != Token::Type::Identifier){
			EXIT("Missing variable or function name");
		}
		const long position = _position;
		const std::string name = current.sVal;
		advance();

		// Basic variable.
		if(match(Operator::Assign)){
			// Parse the rest of the line.
			Result expr = expression();
			EXIT_IF_FAILED(expr);
			return Expression::Ptr(new VariableDef(name, expr, position));
		}

		if(match(Operator::OpenParenth)){
			_parsingFunctionDeclaration = true;

			if(match(Operator::CloseParenth)){
				// Constant function.
				if(!match(Operator::Assign)){
					EXIT("Unexpected character, expected assignment");
				}
				// Parse the rest of the line.
				Result expr = expression();
				EXIT_IF_FAILED(expr);
				return Expression::Ptr(new FunctionDef(name, {}, expr, position));
			}

			// Else parse arguments
			std::vector<std::string> arguments;
			do {
				if(!valid()){
					EXIT("Missing argument name");
				}
				Token& arg = _tokens[_position];
				if(arg.type != Token::Type::Identifier){
					EXIT("Expected argument name");
				}
				advance();
				arguments.emplace_back(arg.sVal);
			} while(match(Operator::Comma));

			if(!match(Operator::CloseParenth)){
				EXIT("Unexpected character, expected parenthesis");
			}
			if(!match(Operator::Assign)){
				EXIT("Unexpected character, expected assignment");
			}
			// Parse the rest of the line.
			Result expr = expression();
			EXIT_IF_FAILED(expr);
			
			return Expression::Ptr(new FunctionDef(name, arguments, expr, position));

		}
		// Else impossible
		EXIT("Incorrect declaration");
	}

	// This is an expression.
	return expression();
}

Parser::Result Parser::expression(){
	return ternary();
}

Parser::Result Parser::ternary(){
	Result condition = boolOr();
	EXIT_IF_FAILED(condition);

	// "?", or just an expression.
	if(!match(Operator::QuestionMark)){
		return condition;
	}

	Result pass = boolOr();
	EXIT_IF_FAILED(pass);

	// ":"
	if(!match(Operator::Colon)){
		// Once we got the "?", this one is mandatory.
		EXIT("Unexpected character, expected colon");
	}

	Result fail = boolOr();
	EXIT_IF_FAILED(fail);

	Expression::Ptr root = std::make_shared<Ternary>(condition, pass, fail, condition->dbgStartPos, fail->dbgEndPos);
	return root;
}

Parser::Result Parser::boolOr(){
	Result left = boolXor();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left;
	while(match(Operator::BoolOr)){
		Result right = boolXor();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::BoolOr, root, right, left->dbgStartPos, right->dbgEndPos);
	}
	return root;
}

Parser::Result Parser::boolXor(){
	Result left = boolAnd();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left;
	while(match(Operator::BoolXor)){
		Result right = boolAnd();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::BoolXor, root, right, left->dbgStartPos, right->dbgEndPos);
	}
	return root;
}

Parser::Result Parser::boolAnd(){
	Result left = bitOr();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left;
	while(match(Operator::BoolAnd)){
		Result right = bitOr();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::BoolAnd, root, right, left->dbgStartPos, right->dbgEndPos);
	}
	return root;
}

Parser::Result Parser::bitOr(){
	Result left = bitXor();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left;
	while(match(Operator::BitOr)){
		Result right = bitXor();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::BitOr, root, right, left->dbgStartPos, right->dbgEndPos);
	}
	return root;
}

Parser::Result Parser::bitXor(){
	Result left = bitAnd();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left;
	while(match(Operator::BitXor)){
		Result right = bitAnd();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::BitXor, root, right, left->dbgStartPos, right->dbgEndPos);
	}
	return root;
}

Parser::Result Parser::bitAnd(){
	Result left = equality();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left;
	while(match(Operator::BitAnd)){
		Result right = equality();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::BitAnd, root, right, left->dbgStartPos, right->dbgEndPos);
	}
	return root;
}

Parser::Result Parser::equality(){
	Result left = comparison();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left;
	while(match({Operator::Equal, Operator::Different})){
		const Operator op = previousOp();
		Result right = comparison();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(op, root, right, left->dbgStartPos, right->dbgEndPos);
	}
	return root;
}

Parser::Result Parser::comparison(){
	Result left = bitshift();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left;
	while(match({Operator::GreaterThan, Operator::GreaterThanEqual, Operator::LessThan, Operator::LessThanEqual})){
		const Operator op = previousOp();
		Result right = bitshift();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(op, root, right, left->dbgStartPos, right->dbgEndPos);
	}
	return root;
}

Parser::Result Parser::bitshift(){
	Result left = term();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left;
	while(match({Operator::ShiftLeft, Operator::ShiftRight})){
		const Operator op = previousOp();
		Result right = term();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(op, root, right, left->dbgStartPos, right->dbgEndPos);
	}
	return root;
}

Parser::Result Parser::term(){
	Result left = factor();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left;
	while(match({Operator::Plus, Operator::Minus})){
		const Operator op = previousOp();
		Result right = factor();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(op, root, right, left->dbgStartPos, right->dbgEndPos);
	}
	return root;
}

Parser::Result Parser::factor(){
	Result left = unary();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left;
	while(match({Operator::Product, Operator::Divide, Operator::Modulo})){
		const Operator op = previousOp();
		Result right = unary();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(op, root, right, left->dbgStartPos, right->dbgEndPos);
	}
	return root;
}

Parser::Result Parser::unary(){
	const long position = _position;
	if(match({Operator::BitNot, Operator::BoolNot, Operator::Plus, Operator::Minus})){
		const Operator op = previousOp();
		Result right = unary();
		EXIT_IF_FAILED(right);

		return Expression::Ptr(new Unary(op, right, position, right->dbgEndPos));
	}

	return power();
}

Parser::Result Parser::power(){
	Result left = member();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left;
	while(match(Operator::Power)){
		Result right = member();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::Power, root, right, left->dbgStartPos, right->dbgEndPos);
	}
	return root;
}

Parser::Result Parser::member(){
	Result parent = terminal();
	EXIT_IF_FAILED(parent);

	Expression::Ptr root = parent;
	while(match(Operator::Dot)){
		if(!valid()){
			EXIT("Missing accessor");
		}
		// Get the next token.
		const Token& member = peek();
		if(member.type != Token::Type::Identifier){
			EXIT("Expected accessor");
		}
		const long position = _position;
		advance();
		root = std::make_shared<Member>(root, member.sVal, position);
	}
	return root;
}

Parser::Result Parser::terminal(){
	if(!valid()){
		EXIT("Missing variable, literal or function call");
	}

	const Token& current = peek();
	const long position = _position;
	if(current.type == Token::Type::Float){
		advance();
		return Expression::Ptr(new Literal(Value(current.fVal), position));
	}
	if(current.type == Token::Type::Integer){
		advance();
		// For now, just store as float.
		return Expression::Ptr(new Literal(Value(current.iVal), position));
	}
	if(current.type == Token::Type::Identifier){
		advance();
		// function call.
		if(match(Operator::OpenParenth)){
			if(match(Operator::CloseParenth)){
				// No arguments.
				return Expression::Ptr(new FunctionCall(current.sVal, {}, position, position));
			}

			// Else parse arguments
			std::vector<Expression::Ptr> arguments;
			do {
				Result arg = expression();
				EXIT_IF_FAILED(arg);
				arguments.push_back(arg);
			} while(match(Operator::Comma));

			const long endPosition = _position;
			if(!match(Operator::CloseParenth)){
				EXIT("Unexpected character, expected parenthesis");
			}

			return Expression::Ptr(new FunctionCall(current.sVal, arguments, position, endPosition));
		} else {
			// Simple variable.
			if(_parsingFunctionDeclaration){
				return Expression::Ptr(new FunctionVar(current.sVal, position));
			} else {
				return Expression::Ptr(new Variable(current.sVal, position));
			}
		}
	}
	// Otherwise, the only valid operator is a left parenthesis.
	if(match(Operator::OpenParenth)){
		Result nested = expression();
		EXIT_IF_FAILED(nested);

		if(!match(Operator::CloseParenth)){
			EXIT("Unexpected character, expected parenthesis");
		}
		return nested;
	}

	EXIT("Invalid variable, function call or literal");
}

Status Parser::parse(){
	_tree = nullptr;
	_parsingFunctionDeclaration = false;

	Result res = statement();
	if(res != nullptr){
		if( _position != _tokenCount){
			// Error (unbalanced parenthesis,...)
			return Status(_tokenCount, "Unbalanced expression");
		}
		_tree = res;
		return Status(true);
	}
	return Status(_failedToken, _failedMessage);
}
