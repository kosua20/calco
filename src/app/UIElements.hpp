#include "core/Common.hpp"
#include "core/Calculator.hpp"

#include <imgui/imgui.h>

struct UILine {

	enum Type {
		INPUT = 0, OUTPUT, ISSUE, EMPTY, COUNT
	};

	UILine(){};

	UILine(Type _type, const std::string& _full) : type(_type), fullText(_full) {}

	struct UIWord {

		UIWord(){}

		UIWord(const std::string& _text, Calculator::Word::Type _type) : text(_text), type(_type) {}

		std::string text;
		Calculator::Word::Type type = Calculator::Word::LITERAL;
	};

	Type type = EMPTY;
	std::vector<UIWord> words;
	std::string fullText;

};


struct UIStyle {

	UIStyle();

	void resetColors();

	void loadFromFile(const std::string& path);

	void saveToFile(const std::string& path);

	ImFont* consoleFont = nullptr;
	ImFont* textFont = nullptr;
	ImVec4 errorColor;
	ImVec4 backgroundColor;
	ImVec4 wordColors[Calculator::Word::COUNT];
	bool displayRowMajor = true;
	static const std::string wordNames[Calculator::Word::COUNT];

};

struct UIState {

	std::vector<UILine> lines;
	std::vector<std::string> commands;
	std::string savedPartialCommand;
	int savedCursor = 0;
	int historyPos = -1;
	std::string textToInsert;
	bool shouldInsert = false;
	bool showVariables = false;
	bool showFunctions = false;
	bool showLibrary = false;

	void saveToStream(std::ostream& str) const;

	void loadFromStream(std::istream& str);

};

