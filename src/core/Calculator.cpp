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
	
	// Evaluate the AST
	Evaluator evaluator(parser.tree());
	std::string log = evaluator.log();
	output = log;

	

	return true;
}
