#pragma once
#include "core/Common.hpp"
#include "core/system/Config.hpp"

class CalcoConfig : public Config {
public:

	explicit CalcoConfig(const std::vector<std::string> & argv) ;

	std::string historyPath = "history.calco";
	std::string settingsPath = "settings.calco";

	// Messages.
	bool version = false;
	bool license = false;
	bool bonus = false;
};
