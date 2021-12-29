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

bool Calculator::evaluate(const std::string& input, std::string& output){
	const std::string& cleanInput = TextUtilities::trim(input, "\t\r\n ");
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
		Evaluator evaluator(varDef->expr, globals);
		const Value res = evaluator.eval();
		globals.setVar(varDef->name, res);
		bool suc;
		const Value resStr = res.convert(Value::STRING, suc);
		output = varDef->name + " = " + resStr.str;
	} else if(auto funDef = std::dynamic_pointer_cast<FunctionDef>(parser.tree())){
		// Unicize names of arguments to avoid collisions later on.
		// Insert current values of all global variables
		output = funDef->name + " defined";
	} else {
		Evaluator evaluator(parser.tree(), globals);
		const Value res = evaluator.eval();
		bool suc;
		const Value resStr = res.convert(Value::STRING, suc);
		output = "= " + resStr.str;

	}

	return true;
}
