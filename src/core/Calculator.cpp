#include "core/Calculator.hpp"
#include "core/Scanner.hpp"
#include "core/Parser.hpp"
#include "core/Evaluator.hpp"
#include "core/system/TextUtilities.hpp"

std::string generateErrorLocationMessage(const std::string& input, int start, int size){
	const std::string errorPointerPadding = (start != 0) ? std::string(start, ' ') : "";
	std::string errorPointer = "^";
	if(size > 2){
		errorPointer.append(std::string(size-2, '-'));
	}
	if(size > 1){
		errorPointer.append("^");
	}

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

bool Calculator::evaluate(const std::string& input, std::string& output, std::vector<SemanticInfo>& infos){
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
			output.append(errorMsg);

		} else {
			// Handle end-of-line errors.
			const Token& lastToken = scanner.tokens().back();
			const std::string errorMsg = generateErrorLocationMessage(cleanInput, lastToken.size, lastToken.size);
			output.append(errorMsg);
		}
		return false;
	}

	// Generate highlighting info.
	const auto& tokens = scanner.tokens();
	const size_t tokenCount = tokens.size();
	infos.resize(tokenCount);

	/// TODO: cleanup syntax generation and API (duplicate enum in the app,...).
	for(size_t tid = 0; tid < tokenCount; ++tid){

		const Token& token = tokens[tid];
		switch(token.type){
			case Token::Type::Operator:
			{
				const bool isSeparator = token.opVal == Operator::OpenParenth || token.opVal == Operator::CloseParenth || token.opVal == Operator::Comma || token.opVal == Operator::Dot;
				infos[tid].type = isSeparator ? SemanticInfo::Type::SEPARATOR : SemanticInfo::Type::OPERATOR;
				break;
			}
			case Token::Type::Identifier:
			{
				if(tid + 1 < tokenCount && tokens[tid+1].type == Token::Type::Operator && tokens[tid+1].opVal == Operator::OpenParenth){
					infos[tid].type =  SemanticInfo::Type::FUNCTION;
				} else {
					infos[tid].type =  SemanticInfo::Type::VARIABLE;
				}
				break;
			}
			case Token::Type::Float:
			case Token::Type::Integer:
			default:
				infos[tid].type = SemanticInfo::Type::LITERAL;
			break;
		}
		infos[tid].location = token.location;
		infos[tid].size = token.size;
	}

	// Three possible cases:
	// * variable definition: evaluate value based on context and store (value+name) in context
	// * function definition: replace existing variables by their value, then store (tree+name+args names) in context
	// * general expression to evaluate: evaluate value based on context and log the result

	// Variable definition
	if(auto varDef = std::dynamic_pointer_cast<VariableDef>(parser.tree())){

		ExpEval evaluator(_globals, _stdlib);
		Value outValue;
		const Status evalResult = varDef->expr->evaluate(evaluator, outValue);

		if(evalResult.success){
			// Store result in global scope.
			_globals.setVar(varDef->name, outValue);
			_globals.setVar("ans", outValue);
			output = varDef->name + " = ";
			const std::string padLines(output.size(), ' ');
			output += outValue.toString(evaluator.getFormat(), padLines);
			return true;

		} else {
			output = "Evaluation: " + evalResult.message + " ";

			const Expression* failExp = evaluator.getErrorExpression();
			if(failExp){
				const Token& firstToken = tokens[failExp->dbgStartPos];
				const Token& lastToken = tokens[failExp->dbgEndPos];
				const long finalSize = lastToken.location - firstToken.location + lastToken.size;
				const std::string errorMsg = generateErrorLocationMessage(cleanInput, firstToken.location, finalSize);
				output.append(errorMsg);
			}
			return false;
		}

	} else if(auto funDef = std::dynamic_pointer_cast<FunctionDef>(parser.tree())){

		// Build unique name for all arguments.
		const std::string suffix = "_" + funDef->name + "_" + std::to_string(_funcCounter);

		// Insert current values of all global variables and unicize arguments names.
		FuncSubstitution flattener(_globals, _stdlib, funDef->args, suffix);
		Value unused;
		const Status evalResult = funDef->expr->evaluate(flattener, unused);

		// Update names list after all substitutions and funcVariable modifications.
		for(std::string& argName : funDef->args){
			argName.append(suffix);
		}

		if(evalResult.success){
			// Store flattened function in global scope.
			_globals.setFunc(funDef->name, funDef);
			output = funDef->name + " defined";
			++_funcCounter;

		} else {
			output = "Evaluation: " + evalResult.message + " ";

			const Expression* failExp = flattener.getErrorExpression();
			if(failExp){
				const Token& firstToken = tokens[failExp->dbgStartPos];
				const Token& lastToken = tokens[failExp->dbgEndPos];
				const long finalSize = lastToken.location - firstToken.location + lastToken.size;
				const std::string errorMsg = generateErrorLocationMessage(cleanInput, firstToken.location, finalSize);
				output.append(errorMsg);
			}
			return false;
		}

	} else {
		ExpEval evaluator(_globals, _stdlib);
		Value outValue;
		const Status evalResult = parser.tree()->evaluate(evaluator, outValue);

		if(evalResult.success){
			// Update ans variable with the last result.
			_globals.setVar("ans", outValue);
			output = "= " + outValue.toString(evaluator.getFormat(), "  ");
			return true;

		} else {
			output = "Evaluation: " + evalResult.message + " ";

			const Expression* failExp = evaluator.getErrorExpression();
			if(failExp){
				const Token& firstToken = tokens[failExp->dbgStartPos];
				const Token& lastToken = tokens[failExp->dbgEndPos];
				const long finalSize = lastToken.location - firstToken.location + lastToken.size;
				const std::string errorMsg = generateErrorLocationMessage(cleanInput, firstToken.location, finalSize);
				output.append(errorMsg);
			}
			return false;

		}

	}

	return true;
}
