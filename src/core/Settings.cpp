#include "core/Settings.hpp"
#include "core/system/TextUtilities.hpp"
#include "core/system/System.hpp"
#include <sr_gui/sr_gui.h>

CalcoConfig::CalcoConfig(const std::vector<std::string> & argv) : Config(argv) {
	// Check if we have custom environment locations.
	char* settingsEnvPath = std::getenv( "CALCO_SETTINGS" );
	if( settingsEnvPath != nullptr )
	{
		settingsPath = std::string( settingsEnvPath );
	}

	char* historyEnvPath = std::getenv( "CALCO_HISTORY" );
	if( historyEnvPath != nullptr )
	{
		historyPath = std::string( historyEnvPath );
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
	registerArgument("settings", "s", "Path to display settings (or use CALCO_SETTINGS environment variable)", "file path");
	registerArgument("history", "h", "Path to a history file (or use CALCO_HISTORY environment variable)", "file path");

	registerSection("Infos");
	registerArgument("version", "v", "Displays the current Calco version.");
	registerArgument("license", "", "Display the license message.");

	// Make sure the two file paths are non empty.
	if( settingsPath.empty() || historyPath.empty() ){
		// Fallback to user data subdirectory
		std::string appDataPath = "";
		char* rawAppDataPath = nullptr;
		if( sr_gui_get_app_data_path( &rawAppDataPath ) == SR_GUI_VALIDATED ){
			appDataPath = std::string( rawAppDataPath );
			free( rawAppDataPath );
		}
		// If no user data directory found, fallback to working dir.
		// Otherwise, create a subdirectory.
		if( !appDataPath.empty() ){
			// Make sure the directory exist
			System::createDirectory( appDataPath );
			appDataPath += "Calco/";
			// Make sure the directory exist
			System::createDirectory( appDataPath );
		}

		if( settingsPath.empty() ){
			settingsPath = appDataPath + "settings.calco";
		}
		if( historyPath.empty() ){
			historyPath = appDataPath + "history.calco";
		}
	}
	// Detect the cases where a directory path was specified.
	if( (settingsPath.back() == '/') || (settingsPath.back() == '\\') ){
		settingsPath += "settings.calco";
	}
	if( (historyPath.back() == '/') || (historyPath.back() == '\\') ){
		historyPath += "history.calco";
	}

	if( historyPath == settingsPath )
	{
		settingsPath += "/settings.calco"; 
		historyPath += "/history.calco";
	}
}
