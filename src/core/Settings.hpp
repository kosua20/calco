#pragma once
#include "core/Common.hpp"
#include "core/system/Config.hpp"

class CalcoConfig : public Config {
public:

	explicit CalcoConfig(const std::vector<std::string> & argv) ;

	std::string historyPath;
	std::string settingsPath;

	// Messages.
	bool version = false;
	bool license = false;
	bool bonus = false;
};
