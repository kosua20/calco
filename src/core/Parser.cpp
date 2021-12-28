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

 statement		→ declaration "=" expression
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

#define EXIT_IF_FAILED(a) if(!a.success){ _failedToken = _failed ? _failedToken : _position; _failed = true; return false;}

const int Parser::NO_ERROR = -1;

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
	/*const long backupPosition = _position;

	Result decl = declaration();
	if(decl.success){
		Result exp = expression();
		exp.tree = std::make_shared<
		return exp;
	} else {
		_position = backupPosition;
		Result exp = expression();
	}
	 */

	return expression();
}

Parser::Result Parser::declaration(){
	// Expect an identifier

	// then either equal (variable, exit)

	// or parenthesis
	// identifier, comma
	// until closing parenthesis
	// then equal (function, exit)
	return false;
}

Parser::Result Parser::expression(){
	return ternary();
}

Parser::Result Parser::ternary(){
	Result condition = boolOr();
	EXIT_IF_FAILED(condition);

	// "?", or just an expression.
	if(!expect(Operator::QuestionMark)){
		return condition;
	}
	advance();

	Result pass = boolOr();
	EXIT_IF_FAILED(pass);

	// ":"
	if(!expect(Operator::Colon)){
		// Once we got the "?", this one is mandatory.
		return false;
	}
	advance();

	Result fail = boolOr();
	EXIT_IF_FAILED(fail);

	Expression::Ptr root = std::make_shared<Ternary>(condition.exp, pass.exp, fail.exp);
	return root;
}

Parser::Result Parser::boolOr(){
	Result left = boolXor();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left.exp;
	while(match(Operator::BoolOr)){
		Result right = boolXor();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::BoolOr, root, right.exp);
	}
	return root;
}

Parser::Result Parser::boolXor(){
	Result left = boolAnd();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left.exp;
	while(match(Operator::BoolXor)){
		Result right = boolAnd();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::BoolXor, root, right.exp);
	}
	return root;
}

Parser::Result Parser::boolAnd(){
	Result left = bitOr();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left.exp;
	while(match(Operator::BoolAnd)){
		Result right = bitOr();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::BoolAnd, root, right.exp);
	}
	return root;
}

Parser::Result Parser::bitOr(){
	Result left = bitXor();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left.exp;
	while(match(Operator::BitOr)){
		Result right = bitXor();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::BitOr, root, right.exp);
	}
	return root;
}

Parser::Result Parser::bitXor(){
	Result left = bitAnd();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left.exp;
	while(match(Operator::BitXor)){
		Result right = bitAnd();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::BitXor, root, right.exp);
	}
	return root;
}

Parser::Result Parser::bitAnd(){
	Result left = equality();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left.exp;
	while(match(Operator::BitAnd)){
		Result right = equality();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::BitAnd, root, right.exp);
	}
	return root;
}

Parser::Result Parser::equality(){
	Result left = comparison();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left.exp;
	while(match({Operator::Equal, Operator::Different})){
		const Operator op = previousOp();
		Result right = comparison();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(op, root, right.exp);
	}
	return root;
}

Parser::Result Parser::comparison(){
	Result left = bitshift();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left.exp;
	while(match({Operator::GreaterThan, Operator::GreaterThanEqual, Operator::LessThan, Operator::LessThanEqual})){
		const Operator op = previousOp();
		Result right = bitshift();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(op, root, right.exp);
	}
	return root;
}

Parser::Result Parser::bitshift(){
	Result left = term();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left.exp;
	while(match({Operator::ShiftLeft, Operator::ShiftRight})){
		const Operator op = previousOp();
		Result right = term();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(op, root, right.exp);
	}
	return root;
}

Parser::Result Parser::term(){
	Result left = factor();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left.exp;
	while(match({Operator::Plus, Operator::Minus})){
		const Operator op = previousOp();
		Result right = factor();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(op, root, right.exp);
	}
	return root;
}

Parser::Result Parser::factor(){
	Result left = unary();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left.exp;
	while(match({Operator::Product, Operator::Divide, Operator::Modulo})){
		const Operator op = previousOp();
		Result right = unary();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(op, root, right.exp);
	}
	return root;
}

Parser::Result Parser::unary(){
	if(match({Operator::BitNot, Operator::BoolNot, Operator::Plus, Operator::Minus})){
		const Operator op = previousOp();
		Result right = unary();
		EXIT_IF_FAILED(right);

		return Expression::Ptr(new Unary(op, right.exp));
	}

	return power();
}

Parser::Result Parser::power(){
	Result left = member();
	EXIT_IF_FAILED(left);

	Expression::Ptr root = left.exp;
	while(match(Operator::Power)){
		Result right = member();
		EXIT_IF_FAILED(right);

		root = std::make_shared<Binary>(Operator::Power, root, right.exp);
	}
	return root;
}

Parser::Result Parser::member(){
	Result parent = terminal();
	EXIT_IF_FAILED(parent);

	Expression::Ptr root = parent.exp;
	while(match(Operator::Dot)){
		// Get the next token.
		const Token& member = peek();
		if(member.type != Token::Type::Identifier){
			return false;
		}
		advance();
		root = std::make_shared<Member>(root, member.sVal);
	}
	return root;
}

Parser::Result Parser::terminal(){

	const Token& current = peek();
	if(current.type == Token::Type::Float){
		advance();
		return Expression::Ptr(new Literal(current.fVal));
	}
	if(current.type == Token::Type::Integer){
		advance();
		// For now, just store as float.
		return Expression::Ptr(new Literal(double(current.iVal)));
	}
	if(current.type == Token::Type::Identifier){
		advance();
		// function call.
		if(match(Operator::OpenParenth)){
			Result arg = expression();
			EXIT_IF_FAILED(arg);

			std::vector<Expression::Ptr> arguments = { arg.exp };

			while(match(Operator::Comma)){
				arg = expression();
				EXIT_IF_FAILED(arg);

				arguments.push_back(arg.exp);
			}
			if(!expect(Operator::CloseParenth)){
				return false;
			}
			advance();
			return Expression::Ptr(new FunctionCall(current.sVal, arguments));
		} else {
			// Simple variable.
			return Expression::Ptr(new Variable(current.sVal));
		}
	}
	// Otherwise, the only valid operator is a left parenthesis.
	if(match(Operator::OpenParenth)){
		Result nested = expression();
		EXIT_IF_FAILED(nested);

		if(!expect(Operator::CloseParenth)){
			return false;
		}
		advance();
		return nested.exp;
	}
	return false;
}

int Parser::parse(){
	_tree = nullptr;
	Result res = statement();
	if(res.success){
		if( _position != _tokenCount){
			// Error (unbalanced parenthesis,...)
			return _tokenCount;
		}
		_tree = res.exp;
		return NO_ERROR;
	}
	return _failedToken;
}
