#include "core/Calculator.hpp"
#include "core/Scanner.hpp"
#include "core/Parser.hpp"
#include "core/Evaluator.hpp"
#include "core/system/TextUtilities.hpp"

void Documentation::setVar(const std::string& name, const Value& value){
	const std::string val = value.toString(_format);
	uint lineCount = 1;
	for(const auto& c : val){
		if(c == '\n'){
			++lineCount;
		}
	}
	_variables[name] = {val, lineCount };
}

void Documentation::setFunc(const std::string& name, const std::shared_ptr<FunctionDef>& def){

	ExpLogger logger;
	// Generate expression.
	std::string expr = def->expr->evaluate(logger).str;
	// Generate name with arguments, removing internal identifiers.
	std::string fullName = def->name + "(";
	std::vector<std::string> args;
	args.resize(def->args.size());

	uint i = 0;
	for(const auto& arg : def->args){
		args[i] = arg.substr(0, arg.find_last_of('@'));
		TextUtilities::replace(expr, arg, args[i]);
		fullName += (i == 0 ? "" : ", ") + args[i];
		++i;
	}
	fullName += ")";

	_functions[name] = { fullName, expr, args };
}

void Documentation::setLibrary(const FunctionsLibrary& library){
	std::unordered_map<std::string, std::string> funcList;
	library.populateDescriptions(funcList);
	for(const auto& func : funcList){
		_stdlib[func.first] = {func.first, func.second};
	}
	// Also do constants here.
	for(const auto& constant : MathConstants){
		_constants[constant.first] = {std::to_string(constant.second), 1};
	}
}

void Documentation::clear(){
	_variables.clear();
	_functions.clear();
	// Never clear the stdlib.
}

std::string generateErrorLocationMessage(const std::string& input, int start, int size){
	const std::string errorPointerPadding = (start != 0) ? std::string(start, ' ') : "";
	std::string errorPointer = std::string(size, '^');

	std::string errorMessage;// = "around position " + std::to_string(start);
	errorMessage.append("\n\t");
	errorMessage.append(input);
	errorMessage.append("\n\t");
	errorMessage.append(errorPointerPadding + errorPointer);
	return errorMessage;
}

std::string logTree(const Expression::Ptr& exp ){
	ExpLogger logger;
	Value finalStr = exp->evaluate(logger);
	return finalStr.str;
}


Calculator::Calculator(){
	_doc.setLibrary(_stdlib);
}

bool Calculator::evaluate(const std::string& input, Value& output, std::vector<Word>& infos, Format& format, bool temporary){
	const std::string& cleanInput = input;

	// Scanning
	Scanner scanner(cleanInput);
	const Status scanResult = scanner.scan();
	if(!scanResult){
		// Point to the problematic character.
		const long characterPos = scanResult.location;
		const std::string errorMsg = generateErrorLocationMessage(cleanInput, characterPos, 1);
		output = "Parsing: " + scanResult.message + " " + errorMsg;
		return false;
	}
	// Build the AST
	Parser parser(scanner.tokens());
	const Status parseResult = parser.parse();
	
	if(!parseResult){
		output = "Compilation: " + parseResult.message + " ";
		if(parseResult.location < int(scanner.tokens().size())){
			const Token& errorToken = scanner.tokens()[parseResult.location];
			const std::string errorMsg = generateErrorLocationMessage(cleanInput, errorToken.location, errorToken.size);
			output.str.append(errorMsg);

		} else {
			// Handle end-of-line errors.
			const Token& lastToken = scanner.tokens().back();
			const std::string errorMsg = generateErrorLocationMessage(cleanInput, lastToken.size, lastToken.size);
			output.str.append(errorMsg);
		}
		return false;
	}

	// Generate highlighting info.
	const auto& tokens = scanner.tokens();

	if(!temporary){
		const size_t tokenCount = tokens.size();
		infos.resize(tokenCount);

		// Classify each token.
		for(size_t tid = 0; tid < tokenCount; ++tid){

			const Token& token = tokens[tid];
			switch(token.type){
				case Token::Type::Operator:
				{
					// Special cases: ( ) , . and unary (operator following an operator or at beginning of line)
					const bool isSeparator = token.opVal == Operator::OpenParenth || token.opVal == Operator::CloseParenth || token.opVal == Operator::Comma || token.opVal == Operator::Dot;
					const bool followOperator = tid == 0 || (tokens[tid-1].type == Token::Type::Operator && tokens[tid-1].opVal != Operator::CloseParenth);
					infos[tid].type = (isSeparator || followOperator) ? Word::SEPARATOR : Word::OPERATOR;
					break;
				}
				case Token::Type::Identifier:
				{
					// If followed by a parenthesis, function. Otherwise, variable.
					if(tid + 1 < tokenCount && tokens[tid+1].type == Token::Type::Operator && tokens[tid+1].opVal == Operator::OpenParenth){
						infos[tid].type =  Word::FUNCTION;
					} else {
						infos[tid].type =  Word::VARIABLE;
					}
					break;
				}
				case Token::Type::Float:
				case Token::Type::Integer:
				default:
					infos[tid].type = Word::LITERAL;
				break;
			}
			infos[tid].location = token.location;
			infos[tid].size = token.size;
		}
	}

	// Three possible cases:
	// * variable definition: evaluate value based on context and store (value+name) in context
	// * function definition: replace existing variables by their value, then store (tree+name+args names) in context
	// * general expression to evaluate: evaluate value based on context and log the result

	// Variable definition
	if(auto varDef = std::dynamic_pointer_cast<VariableDef>(parser.tree())){

		ExpEval evaluator(_globals, _stdlib, format);
		Value outValue;
		const Status evalResult = varDef->expr->evaluate(evaluator, outValue);

		if(evalResult.success){

			output = outValue;
			format = evaluator.getFormat();

			if(!temporary){
				// Store result in global scope.
				_globals.setVar(varDef->name, outValue);
				_globals.setVar("ans", outValue);
				// Register variable name for display.
				_doc.setVar(varDef->name, outValue);
				_doc.setVar("ans", outValue);
			}
			return true;

		} else {
			output = "Evaluation: " + evalResult.message + " ";

			const Expression* failExp = evaluator.getErrorExpression();
			if(failExp){
				const Token& firstToken = tokens[failExp->dbgStartPos];
				const Token& lastToken = tokens[failExp->dbgEndPos];
				const long finalSize = lastToken.location - firstToken.location + lastToken.size;
				const std::string errorMsg = generateErrorLocationMessage(cleanInput, firstToken.location, finalSize);
				output.str.append(errorMsg);
			}
			return false;
		}

	} else if(auto funDef = std::dynamic_pointer_cast<FunctionDef>(parser.tree())){

		// Build unique name for all arguments.
		const std::string suffix = "@" + funDef->name + "_" + (temporary ? "tmp" : std::to_string(_funcCounter));

		// Insert current values of all global variables and unicize arguments names.
		FuncSubstitution flattener(_globals, _stdlib, funDef->args, suffix);
		Value unused;
		const Status evalResult = funDef->expr->evaluate(flattener, unused);

		// Update names list after all substitutions and funcVariable modifications.
		for(std::string& argName : funDef->args){
			argName.append(suffix);
		}

		if(evalResult.success){
			output = funDef->name;

			if(!temporary){
				++_funcCounter;
				// Store flattened function in global scope.
				_globals.setFunc(funDef->name, funDef);
				// Register function name for display.
				_doc.setFunc(funDef->name, funDef);
			}
			return true;
		} else {
			output = "Evaluation: " + evalResult.message + " ";

			const Expression* failExp = flattener.getErrorExpression();
			if(failExp){
				const Token& firstToken = tokens[failExp->dbgStartPos];
				const Token& lastToken = tokens[failExp->dbgEndPos];
				const long finalSize = lastToken.location - firstToken.location + lastToken.size;
				const std::string errorMsg = generateErrorLocationMessage(cleanInput, firstToken.location, finalSize);
				output.str.append(errorMsg);
			}
			return false;
		}

	} else {
		ExpEval evaluator(_globals, _stdlib, format);
		Value outValue;
		const Status evalResult = parser.tree()->evaluate(evaluator, outValue);

		if(evalResult.success){
			output = outValue;
			format = evaluator.getFormat();

			if(!temporary){
				// Update ans variable with the last result.
				_globals.setVar("ans", outValue);
				_doc.setVar("ans", outValue);
			}
			return true;

		} else {
			output = "Evaluation: " + evalResult.message + " ";

			const Expression* failExp = evaluator.getErrorExpression();
			if(failExp){
				const Token& firstToken = tokens[failExp->dbgStartPos];
				const Token& lastToken = tokens[failExp->dbgEndPos];
				const long finalSize = lastToken.location - firstToken.location + lastToken.size;
				const std::string errorMsg = generateErrorLocationMessage(cleanInput, firstToken.location, finalSize);
				output.str.append(errorMsg);
			}
			return false;

		}
	}

	return true;
}

bool Calculator::evaluateFunction(const std::string& name, const std::vector<Value>& args, Value& output){
	
	// Build a function call.
	const size_t argCount = args.size();
	std::vector<Expression::Ptr> argValues(argCount);
	for(size_t aid = 0; aid < argCount; ++aid){
		argValues[aid] = std::make_shared<Literal>(args[aid], (long)aid);
	}
	const auto func = std::make_shared<FunctionCall>(name, argValues, 0, 1);
	Format format = Format::INTERNAL;
	ExpEval eval(_globals, _stdlib, format);
	output = func->evaluate(eval);
	return true;
}

void Calculator::clear(){
	_globals = Scope();
	_funcCounter = 0;
	_doc.clear();
}

void Calculator::updateDocumentation(Format format){
	_doc.setFormat(format);
	// Should we clear first?
	// _doc.clear();

	/// Register all variables
	for (const auto& variable : _globals.getVars()) {
		_doc.setVar(variable.first, variable.second);
	}

	/// Register all functions.
	for (const auto& function : _globals.getFuncs()) {
		_doc.setFunc(function.first, function.second);
	}
}

void Calculator::saveToStream(std::ostream& str) const {
	// Don't need to save funCounter.
	const auto& variables = _globals.getVars();
	const auto& functions = _globals.getFuncs();

	str << "CALCSTATE" << "\n";
	str << "VARIABLES " << int(variables.size()) << "\n";
	for (const auto& variable : variables) {
		str << variable.first << " = " << variable.second.toString(Format::INTERNAL) << "\n";
	}
	str << "FUNCTIONS " << int(functions.size()) << "\n";
	ExpLogger logger;
	for (const auto& function : functions) {
		str << function.second->evaluate(logger).str << "\n";
	}
}


void Calculator::loadFromStream(std::istream& str){
	Scope emptyGlobal;
	ExpEval sharedEval(emptyGlobal, _stdlib, Format::INTERNAL);

	// Assume CALCSTATE has just been read.
	std::string dfltStr;
	int count;
	str >> dfltStr >> count;
	assert(dfltStr == "VARIABLES");
	std::getline(str, dfltStr);

	for (int i = 0; i < count; ++i) {
		std::string varExp;
		std::getline(str, varExp);

		Scanner scanner(varExp);
		const Status scanResult = scanner.scan();
		if (!scanResult) {
			continue;
		}
		// Build the AST
		Parser parser(scanner.tokens());
		const Status parseResult = parser.parse();
		if (!parseResult) {
			continue;
		}

		auto varDef = std::dynamic_pointer_cast<VariableDef>(parser.tree());
		Value outValue;
		const Status evalResult = varDef->expr->evaluate(sharedEval, outValue);

		if (evalResult.success) {
			_globals.setVar(varDef->name, outValue);
		}
	}

	str >> dfltStr >> count;
	assert(dfltStr == "FUNCTIONS");
	std::getline(str, dfltStr);
	for (int i = 0; i < count; ++i) {
		std::string funcExpr;
		std::getline(str, funcExpr);

		Scanner scanner(funcExpr);
		const Status scanResult = scanner.scan();
		if (!scanResult) {
			continue;
		}
		// Build the AST
		Parser parser(scanner.tokens());
		const Status parseResult = parser.parse();
		if (!parseResult) {
			continue;
		}

		auto funDef = std::dynamic_pointer_cast<FunctionDef>(parser.tree());
		_globals.setFunc(funDef->name, funDef);

	}

	updateDocumentation(_doc.format());
}
