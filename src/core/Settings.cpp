#include "core/Settings.hpp"
#include "core/system/TextUtilities.hpp"

CalcoConfig::CalcoConfig(const std::vector<std::string> & argv) : Config(argv) {

	char* settingsEnvPath = std::getenv("CALCO_SETTINGS");
	if(settingsEnvPath != nullptr){
		settingsPath = std::string(settingsEnvPath);
	}

	char* historyEnvPath = std::getenv("CALCO_HISTORY");
	if(historyEnvPath != nullptr){
		historyPath = std::string(historyEnvPath);
	}

	// Process arguments.
	for(const auto & arg : arguments()) {
		// Config path.
		if((arg.key == "settings" || arg.key == "s") && !arg.values.empty()){
			settingsPath = arg.values[0];
		}
		if((arg.key == "history" || arg.key == "h") && !arg.values.empty()){
			historyPath = arg.values[0];
		}

		if(arg.key == "version" || arg.key == "v") {
			version = true;
		}
		if(arg.key == "license") {
			license = true;
		}

		if(arg.key == "coucou") {
			bonus = true;
		}

	}

	registerSection("Settings");
	registerArgument("settings", "s", "Path to display settings", "file path");
	registerArgument("history", "h", "Path to a history file", "file path");

	registerSection("Infos");
	registerArgument("version", "v", "Displays the current Calco version.");
	registerArgument("license", "", "Display the license message.");

}
