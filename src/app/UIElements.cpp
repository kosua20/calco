#include "UIElements.hpp"

#include "core/system/TextUtilities.hpp"


const std::string UIStyle::wordNames[] = {
	"LITERAL", "VARIABLE", "FUNCTION", "OPERATOR", "SEPARATOR", "RESULT"
};

UIStyle::UIStyle() {
	format = Format(Format::BASE_10_FLAG | Format::MAJOR_ROW_FLAG);
	resetColors();
}

void UIStyle::resetColors() {
	backgroundColor = ImVec4(0.137255f, 0.031373f, 0.105882f, 1.0f);
	errorColor = ImVec4(0.8f, 0.2f, 0.1f, 1.0f);
	wordColors[Calculator::Word::LITERAL] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	wordColors[Calculator::Word::VARIABLE] = ImVec4(0.258824f, 0.545098f, 0.000000f, 1.0f);
	wordColors[Calculator::Word::FUNCTION] = ImVec4(0.635294f, 0.835294f, 0.113725f, 1.0f);
	wordColors[Calculator::Word::OPERATOR] = ImVec4(0.713726f, 0.560784f, 0.000000f, 1.0f);
	wordColors[Calculator::Word::SEPARATOR] = ImVec4(0.603922f, 0.415686f, 0.600000f, 1.0f);
	wordColors[Calculator::Word::RESULT] = ImVec4(0.349020f, 0.556863f, 0.776471f, 1.0f);
}

void UIStyle::loadFromFile(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		Log::Error() << "Unable to load settings from file \"" << path << "\"" << std::endl;
		return;
	}
	std::string line;
	while (std::getline(file, line)) {
		const std::vector<std::string>& lineElements = TextUtilities::split(line, " ", true);
		if (lineElements.size() < 2) {
			continue;
		}
		const std::string& key = lineElements[0];
		if (key == "FORMAT") {
			format = Format(std::stoul(lineElements[1]));
			continue;
		}
		// all other are colors
		if (lineElements.size() < 4) {
			Log::Error() << "Missing parameters for keyword \"" << key << "\"" << std::endl;
			continue;
		}
		ImVec4 color(1.f, 1.f, 1.f, 1.f);
		color.x = std::stof(lineElements[1]);
		color.y = std::stof(lineElements[2]);
		color.z = std::stof(lineElements[3]);

		if (key == "BACKGROUND") {
			backgroundColor = color;
			continue;
		}
		else if (key == "ERROR") {
			errorColor = color;
			continue;
		}
		bool used = false;
		for (int i = 0; i < Calculator::Word::COUNT; ++i) {
			if (key == wordNames[i]) {
				used = true;
				wordColors[i] = color;
				break;
			}
		}

		if (used) {
			continue;
		}
		Log::Error() << "Unknown keyword \"" << key << "\"" << std::endl;
	}
	file.close();
}

void UIStyle::saveToFile(const std::string& path) {
	std::ofstream file(path);
	if (!file.is_open()) {
		Log::Error() << "Unable to save settings to file \"" << path << "\"" << std::endl;
		return;
	}

	file << "BACKGROUND " << backgroundColor.x << " " << backgroundColor.y << " " << backgroundColor.z << "\n";
	file << "ERROR " << errorColor.x << " " << errorColor.y << " " << errorColor.z << "\n";
	file << "FORMAT " << (unsigned long)format << "\n";
	for (uint i = 0; i < Calculator::Word::COUNT; ++i) {
		file << wordNames[i] << " " << wordColors[i].x << " " << wordColors[i].y << " " << wordColors[i].z << "\n";
	}
	file.close();
}

void UIState::saveToStream(std::ostream& str) const {
	str << "UISTATE" << "\n";
	str << "LINES " << int(lines.size()) << "\n";
	for (const auto& line : lines) {
		str << int(line.type) << " " << int(line.words.size()) << "\n";
		for (const auto& word : line.words) {
			str << int(word.type) << " " << word.text << "\n";
		}
		str << line.fullText << "\n";
	}
	str << "COMMANDS " << commands.size() << "\n";
	for (const auto& command : commands) {
		str << command << "\n";
	}
}

void UIState::loadFromStream(std::istream& str) {
	// Assume we just read UISTATE
	std::string dfltStr;
	int count = 0;

	str >> dfltStr >> count;
	assert(dfltStr == "LINES");
	lines.resize(count);

	for (int i = 0; i < count; ++i) {

		int type; int wordCount;
		str >> type >> wordCount;
		lines[i].type = UILine::Type(type);
		lines[i].words.resize(wordCount);

		for (int j = 0; j < wordCount; ++j) {
			str >> type;
			lines[i].words[j].type = Calculator::Word::Type(type);

			std::getline(str, dfltStr);
			// Prefix space has not been absorbed.
			lines[i].words[j].text = dfltStr.substr(1);
		}

		// Absorb \n
		if (wordCount == 0) {
			std::getline(str, dfltStr);
		}

		if (!std::getline(str, lines[i].fullText)) {
			Log::Error() << "Error parsing state lines." << std::endl;
			return;
		}
	}

	str >> dfltStr >> count;
	assert(dfltStr == "COMMANDS");
	commands.resize(count);
	std::getline(str, dfltStr);

	for (int i = 0; i < count; ++i) {
		if (!std::getline(str, commands[i])) {
			Log::Error() << "Error parsing state commands." << std::endl;
			return;
		}
	}
}

