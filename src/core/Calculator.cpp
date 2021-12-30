#include "core/Calculator.hpp"
#include "core/Scanner.hpp"
#include "core/Parser.hpp"
#include "core/Evaluator.hpp"
#include "core/system/TextUtilities.hpp"

std::string generateErrorLocationMessage(const std::string& input, int start, int end){
	const std::string errorPointerPadding = (start != 0) ? std::string(start, ' ') : "";
	const std::string errorPointer = std::string(end - start + 1, '^');

	std::string errorMessage = "around position " + std::to_string(start);
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

bool Calculator::evaluate(const std::string& input, std::string& output){
	const std::string& cleanInput = input;
	// Scanning
	Scanner scanner(cleanInput);
	const Status scanResult = scanner.scan();
	if(!scanResult){
		// Point to the problematic character.
		const long characterPos = scanResult.location;
		const std::string errorMsg = generateErrorLocationMessage(cleanInput, characterPos, characterPos);
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
			const std::string errorMsg = generateErrorLocationMessage(cleanInput, errorToken.dbgStartPos, errorToken.dbgEndPos);
			output.append(errorMsg);

		} else {
			// Handle end-of-line errors.
			const Token& lastToken = scanner.tokens().back();
			const std::string errorMsg = generateErrorLocationMessage(cleanInput, lastToken.dbgEndPos, lastToken.dbgEndPos);
			output.append(errorMsg);
		}
		return false;
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
			output = varDef->name + " = " + outValue.toString();
			return true;
		} else {
			const Expression* failExp = evaluator.getErrorExpression();
			output = "Evaluation error: " + evalResult.message;
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
			const Expression* failExp = flattener.getErrorExpression();
			output = "Evaluation error: " + evalResult.message;
			return false;
		}

	} else {
		ExpEval evaluator(_globals, _stdlib);
		Value outValue;
		const Status evalResult = parser.tree()->evaluate(evaluator, outValue);

		if(evalResult.success){
			output = "= " + outValue.toString();
			return true;
		} else {
			const Expression* failExp = evaluator.getErrorExpression();
			output = "Evaluation error: " + evalResult.message;
			return false;
		}

	}

	return true;
}
