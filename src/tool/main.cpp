#include "core/Common.hpp"
#include "core/Strings.hpp"
#include "core/Settings.hpp"
#include "core/Calculator.hpp"
#include "core/system/TextUtilities.hpp"

#include <iostream>


bool queryLineFromCIN(char* buffer, int size) {
	// Wait for some input.
	int lenRead = 0;
	buffer[0]	= '\0';
	// Read everything there is. The first line is our result, the rest should be absorbed.
	if(fgets(buffer, size - 1, stdin) == NULL) {
		return false;
	}
	// Remove endlines.
	char* endLine = strchr(buffer, '\n');
	if(endLine) {
		*endLine = '\0';
	}
	endLine = strchr(buffer, '\r');
	if(endLine) {
		*endLine = '\0';
	}
	lenRead = (int)strlen(buffer);
	// Force end of line
	buffer[size-1] = '\0';
	return lenRead;
}

int main(int argc, char** argv) {

	CalcoConfig config(std::vector<std::string>(argv, argv+argc));
	if(config.version){
		Log::Info() << versionMessage << std::endl;
		return 0;
	} else if(config.license){
		Log::Info() << licenseMessage << std::endl;
		return 0;
	} else if(config.bonus){
		Log::Info() << bonusMessage << std::endl;
		return 0;
	} else if(config.showHelp(false)){
		return 0;
	}

	Calculator calculator;

	// Save/restore calculator state
	{
		std::ifstream file(config.historyPath);
		if (file.is_open()) {
			std::string elem;
			file >> elem;
			if (elem == "CALCSTATE") {
				calculator.loadFromStream(file);
				file >> elem;
			}
			file.close();
		} else {
			Log::Error() << "Unable to open state at \"" << config.historyPath << "\"" << std::endl;
		}
	}

	// TODO: support specifying history at the same time.
	std::string inputLine;
	for(int i = 1; i < argc; ++i){
		inputLine += std::string(argv[i]) + (i != (argc - 1 )? " " : "");
	}

	if(inputLine == "functions"){
		calculator.updateDocumentation(Format::INTERNAL);
		std::cout << "--------------------------------------------------\n";
		std::cout << "Functions: \n";
		std::cout << "--------------------------------------------------\n";
		for(const auto& func : calculator.functions()){
			std::cout << func.second.name << " = " << func.second.expression << "\n";
		}
		std::cout << "--------------------------------------------------\n" << std::flush;
		// No need to save the state.
		return 0;
	} else if(inputLine == "variables"){
		calculator.updateDocumentation(Format::INTERNAL);
		std::cout << "--------------------------------------------------\n";
		std::cout << "Variables: \n";
		std::cout << "--------------------------------------------------\n";
		for(const auto& var : calculator.variables()){
			std::cout << var.first << " = " << var.second.value << "\n";
		}
		std::cout << "--------------------------------------------------\n" << std::flush;
		// No need to save the state.
		return 0;
	} else if(inputLine == "library"){
		calculator.updateDocumentation(Format::INTERNAL);
		std::cout << "--------------------------------------------------\n";
		std::cout << "Standard functions: \n";
		std::cout << "--------------------------------------------------\n";
		for(const auto& func : calculator.stdlib()){
			std::cout << func.second.name << ": " << func.second.expression << "\n";
		}
		std::cout << "--------------------------------------------------\n" << std::flush;
		// No need to save the state.
		return 0;
	} else if(inputLine == "constants"){
		calculator.updateDocumentation(Format::INTERNAL);
		std::cout << "--------------------------------------------------\n";
		std::cout << "Constants: \n";
		std::cout << "--------------------------------------------------\n";
		for(const auto& var : calculator.constants()){
			std::cout << var.first << " = " << var.second.value << "\n";
		}
		std::cout << "--------------------------------------------------\n" << std::flush;
		// No need to save the state.
		return 0;
	}

	// Else, real expression.

	Value result;
	Format format = Format::INTERNAL;
	std::vector<Calculator::Word> wordInfos;
	const bool success = calculator.evaluate(inputLine, result, wordInfos, format, false);

	// Input line, with syntax highlighted words.
	std::string inputLineInterpreted;
	bool first = true;
	for(const Calculator::Word& word : wordInfos){
		std::string wordString = inputLine.substr(word.location, word.size);
		if(word.type == Calculator::Word::OPERATOR && !first){
			wordString = " " + wordString + " ";
		}
		inputLineInterpreted.append(wordString);
		first = false;
	}

	std::cout << inputLineInterpreted << "\n" << std::flush;

	// Output/error line
	if(!success){
		// Error line: passthrough the error message from the calculator.
		// The message can be multi-lines, split it.
		const std::vector<std::string> sublines = TextUtilities::split(result.str, "\n", false);
		std::cerr << "Error:\n";
		for (const std::string& subline : sublines) {
			std::cerr << subline << "\n";
		}
		std::cerr << std::flush;
		return 1;
	} else if(result.type == Value::Type::STRING){
		// This is 'function definition' specific.
		std::cout << result.str << " defined" << "\n";

	} else {

		// Build final display properly formatted.
		const std::string externalStr = result.toString(format);
		// The message can be multi-lines, split it.
		const std::vector<std::string> sublines = TextUtilities::split(externalStr, "\n", false);
		bool firstl = true;
		for (const std::string& subline : sublines) {
			std::cout << (firstl ? "= " : "  ") << subline << "\n";
			firstl = false;
		}

	}
	std::cout << std::flush;

	{
		std::ofstream file(config.historyPath);
		if(file.is_open()) {
			calculator.saveToStream(file);
			file.close();
		} else {
			Log::Error() << "Unable to save state to \"" << config.historyPath << "\"" << std::endl;
		}

	}

	return 0;
}
