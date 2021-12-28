#include "core/Calculator.hpp"
#include "core/Scanner.hpp"
#include "core/Parser.hpp"
#include "core/Evaluator.hpp"
#include "core/system/TextUtilities.hpp"



bool Calculator::evaluate(const std::string& input, std::string& output){
	const std::string& cleanInput = TextUtilities::trim(input, "\t\r\n ");
	// Scanning
	Scanner scanner(cleanInput);
	const int scanResult = scanner.scan();
	if(scanResult != Scanner::NO_ERROR){
		const std::string errorPointerPadding = scanResult != 0 ? std::string(scanResult, ' ') : "";
		std::string errorMessage = "Parsing error around position " + std::to_string(scanResult);
		errorMessage.append("\n\t");
		errorMessage.append(cleanInput);
		errorMessage.append("\n\t");
		errorMessage.append(errorPointerPadding + "^");
		output = errorMessage;
		return false;
	}
	// Build the AST
	Parser parser(scanner.tokens());
	const int parseResult = parser.parse();
	if(parseResult != Parser::NO_ERROR){
		long tokenStart = 0;
		long tokenLength = 0;
		if(parseResult < int(scanner.tokens().size())){
			const Token& errorToken = scanner.tokens()[parseResult];
			tokenLength = errorToken.dbgEndPos - errorToken.dbgStartPos + 1;
			tokenStart = errorToken.dbgStartPos;
		} else {
			// Handle end-of-line errors.
			const Token& errorToken = scanner.tokens().back();
			tokenLength = 1;
			tokenStart = errorToken.dbgEndPos+1;
		}

		const std::string errorPointerPadding = (tokenStart != 0) ? std::string(tokenStart, ' ') : "";
		const std::string errorPointer = std::string(tokenLength, '^');

		std::string errorMessage = "Compilation error around position " + std::to_string(tokenStart);
		errorMessage.append("\n\t");
		errorMessage.append(cleanInput);
		errorMessage.append("\n\t");
		errorMessage.append(errorPointerPadding + errorPointer);
		output = errorMessage;
		return false;
	}
	
	// Evaluate the AST
	Evaluator evaluator(parser.tree());
	std::string log = evaluator.log();
	output = log;

	// Debug token log.
	/*std::string log;
	for(const Token& tok : scanner.tokens()){
		if(tok.type == Token::Type::Float){
			log += " " + std::to_string(tok.fVal) + "f";
		} else if(tok.type == Token::Type::Integer){
			log += " " + std::to_string(tok.iVal) + "i";
		} else if(tok.type == Token::Type::Operator){
			log += " " + OperatorString(tok.opVal);
		} else if(tok.type == Token::Type::Identifier){
			log += " " + tok.sVal;
		}
	}
	output = "= " + log;*/

	return true;
}
