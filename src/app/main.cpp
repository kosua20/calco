#include "core/Common.hpp"
#include "core/Strings.hpp"
#include "core/Calculator.hpp"
#include "core/Settings.hpp"

#include "core/system/Config.hpp"
#include "core/system/System.hpp"
#include "core/system/TextUtilities.hpp"

#include <gl3w/gl3w.h>
#include <GLFW/glfw3.h>
#include <sr_gui/sr_gui.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <unordered_map>

#include "font_data_Inconsolata.h"
#include "font_data_Lato.h"


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

	UIStyle(){
		resetColors();
	}

	void resetColors(){
		backgroundColor 						= ImVec4(0.137255f, 0.031373f, 0.105882f, 1.0f);
		errorColor 								= ImVec4(0.8f, 0.2f, 0.1f, 1.0f);
		wordColors[Calculator::Word::LITERAL] 	= ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		wordColors[Calculator::Word::VARIABLE] 	= ImVec4(0.258824f, 0.545098f, 0.000000f, 1.0f);
		wordColors[Calculator::Word::FUNCTION] 	= ImVec4(0.635294f, 0.835294f, 0.113725f, 1.0f);
		wordColors[Calculator::Word::OPERATOR] 	= ImVec4(0.713726f, 0.560784f, 0.000000f, 1.0f);
		wordColors[Calculator::Word::SEPARATOR] = ImVec4(0.603922f, 0.415686f, 0.600000f, 1.0f);
		wordColors[Calculator::Word::RESULT] 	= ImVec4(0.349020f, 0.556863f, 0.776471f, 1.0f);
	}

	void loadFromFile(const std::string& path){
		std::ifstream file(path);
		if(!file.is_open()){
			Log::Error() << "Unable to load settings from file \"" << path << "\"" << std::endl;
			return;
		}
		std::string line;
		while(std::getline(file, line)){
			const std::vector<std::string>& lineElements = TextUtilities::split(line, " ", true);
			if(lineElements.size() < 2){
				continue;
			}
			const std::string& key = lineElements[0];
			if(key == "ROWMAJOR"){
				displayRowMajor = std::stoi(lineElements[1]) != 0;
				continue;
			}
			// all other are colors
			if(lineElements.size() < 4){
				Log::Error() << "Missing parameters for keyword \"" << key << "\"" << std::endl;
				continue;
			}
			ImVec4 color(1.f,1.f,1.f,1.f);
			color.x = std::stof(lineElements[1]);
			color.y = std::stof(lineElements[2]);
			color.z = std::stof(lineElements[3]);

			if(key == "BACKGROUND"){
				backgroundColor = color;
				continue;
			} else if(key == "ERROR"){
				errorColor = color;
				continue;
			}
			bool used = false;
			for(int i = 0; i < Calculator::Word::COUNT; ++i){
				if(key == wordNames[i]){
					used = true;
					wordColors[i] = color;
					break;
				}
			}

			if(used){
				continue;
			}
			Log::Error() << "Unknown keyword \"" << key << "\"" << std::endl;
		}
		file.close();
	}

	void saveToFile(const std::string& path){
		std::ofstream file(path);
		if(!file.is_open()){
			Log::Error() << "Unable to save settings to file \"" << path << "\"" << std::endl;
			return;
		}

		file << "BACKGROUND " << backgroundColor.x << " " << backgroundColor.y << " " << backgroundColor.z << "\n";
		file << "ERROR " << errorColor.x << " " << errorColor.y << " " << errorColor.z << "\n";
		file << "ROWMAJOR " << (displayRowMajor ? 1 : 0) << "\n";
		for(uint i = 0; i < Calculator::Word::COUNT; ++i){
			file << wordNames[i] << " " << wordColors[i].x << " " << wordColors[i].y << " " << wordColors[i].z << "\n";
		}
		file.close();
	}

	ImFont* consoleFont = nullptr;
	ImFont* textFont = nullptr;
	ImVec4 errorColor;
	ImVec4 backgroundColor;
	ImVec4 wordColors[Calculator::Word::COUNT];
	bool displayRowMajor = true;
	static const std::string wordNames[Calculator::Word::COUNT];

};

const std::string UIStyle::wordNames[] = {
	"LITERAL", "VARIABLE", "FUNCTION", "OPERATOR", "SEPARATOR", "RESULT"
};

struct UIState {
	std::vector<UILine> lines;
	std::vector<std::string> commands;
	std::string savedPartialCommand;
	int savedCursor = 0;
	int historyPos = -1;
	std::string textToInsert;
	bool shouldInsert = false;

	void saveToStream(std::ostream& str){
		str << "UISTATE" << "\n";
		str << "LINES " << int(lines.size()) << "\n";
		for(const auto& line : lines){
			str << int(line.type) << " " << int(line.words.size()) << "\n";
			for(const auto& word : line.words){
				str << int(word.type) << " " << word.text << "\n";
			}
			str << line.fullText << "\n";
		}
		str << "COMMANDS " << commands.size() << "\n";
		for(const auto& command : commands){
			str << command << "\n";
		}
	}

	void loadFromStream(std::istream& str){
		// Assume we just read UISTATE
		std::string dfltStr;
		int count = 0;

		str >> dfltStr >> count;
		assert(dfltStr == "LINES");
		lines.resize(count);

		for(int i = 0; i < count; ++i){

			int type; int wordCount;
			str >> type >> wordCount;
			lines[i].type = UILine::Type(type);
			lines[i].words.resize(wordCount);

			for(int j = 0; j < wordCount; ++j){
				str >> type;
				lines[i].words[j].type = Calculator::Word::Type( type );

				std::getline(str, dfltStr);
				// Prefix space has not been absorbed.
				lines[i].words[j].text = dfltStr.substr(1);
			}

			// Absorb \n
			if(wordCount == 0){
				std::getline(str, dfltStr);
			}

			if(!std::getline(str, lines[i].fullText)){
				Log::Error() << "Error parsing state lines." << std::endl;
				return;
			}
		}

		str >> dfltStr >> count;
		assert(dfltStr == "COMMANDS");
		commands.resize(count);
		std::getline(str, dfltStr);
		
		for(int i = 0; i < count; ++i){
			if(!std::getline(str, commands[i])){
				Log::Error() << "Error parsing state commands." << std::endl;
				return;
			}
		}
	}

};

GLFWwindow* createWindow(int w, int h, UIStyle& uiStyle) {

	// Initialize glfw, which will create and setup an OpenGL context.
	if(!glfwInit()) {
		return NULL;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	const unsigned int ww = std::max( mode->width/2, w);
	const unsigned int hh = std::max( mode->height/2, h);

	GLFWwindow* window = glfwCreateWindow(ww, hh, "Calco", NULL, NULL);

	if(!window) {
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);

	if(gl3wInit()) {
		return NULL;
	}
	if(!gl3wIsSupported(3, 2)) {
		return NULL;
	}

	glfwSwapInterval(1);

	ImGui::CreateContext();

	ImFontConfig fontLato = ImFontConfig();
	fontLato.FontData = (void*)(fontDataLato);
	fontLato.FontDataSize = size_fontDataLato;
	fontLato.SizePixels = 18.0f;
	// Font data is static
	fontLato.FontDataOwnedByAtlas = false;
	ImFontConfig fontConsole = ImFontConfig();
	fontConsole.FontData = (void*)(fontDataInconsolata);
	fontConsole.FontDataSize = size_fontDataInconsolata;
	fontConsole.SizePixels = 18.0f;
	// Font data is static
	fontConsole.FontDataOwnedByAtlas = false;

	ImGuiIO & io = ImGui::GetIO();
	uiStyle.textFont = io.Fonts->AddFont(&fontLato);
	uiStyle.consoleFont = io.Fonts->AddFont(&fontConsole);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ImVec2(8,8);
	style.FramePadding = ImVec2(10,4);
	style.CellPadding = ImVec2(4,2);
	style.ItemSpacing = ImVec2(10,10);
	style.ItemInnerSpacing = ImVec2(4,4);
	style.GrabMinSize = 18;
	style.FrameBorderSize = 0;
	style.WindowBorderSize = 0;
	style.FrameRounding = 4;
	style.GrabRounding = 12;
	style.PopupBorderSize = 0;
	style.PopupRounding = 3;
	style.WindowRounding = 5;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_FrameBg]                = ImVec4(0.58f, 0.58f, 0.58f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.72f, 0.72f, 0.72f, 0.40f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.45f, 0.45f, 0.45f, 0.40f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.63f, 0.63f, 0.63f, 1.00f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(1.00f, 1.00f, 1.00F, 0.16f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(1.00f, 1.00f, 1.00F, 0.16f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(1.00f, 1.00f, 1.00F, 0.28f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
	colors[ImGuiCol_PopupBg]           		= ImVec4(0.15f, 0.15f, 0.15f, 0.94f);

	return window;
}

int textCallback(ImGuiInputTextCallbackData* data){
	UIState& state = *((UIState*)(data->UserData));

	// Insert extra text if requested.
	if(state.shouldInsert){
		data->InsertChars(data->CursorPos, state.textToInsert.c_str());
		state.shouldInsert = false;
		state.textToInsert = "";
	}

	switch (data->EventFlag) {
		case ImGuiInputTextFlags_CallbackHistory:
		{
			const int prevHistoryPos = state.historyPos;
			const int historySize = int(state.commands.size());
			if(data->EventKey == ImGuiKey_UpArrow){
				if(state.historyPos == -1){
					state.historyPos = std::max(historySize - 1, 0);
					// Save the current command.
					state.savedPartialCommand = std::string(data->Buf, data->BufTextLen);
					state.savedCursor = data->CursorPos;
				} else if(state.historyPos > 0){
					--state.historyPos;
				}
			} else if(data->EventKey == ImGuiKey_DownArrow) {
				if(state.historyPos != -1){
					++state.historyPos;
				}
			}
			if(state.historyPos >= historySize){
				state.historyPos = -1;
			}

			if(prevHistoryPos != state.historyPos){
				if(state.historyPos != -1){
					// Update with previous command.
					data->DeleteChars(0, data->BufTextLen);
					data->InsertChars(0, state.commands[state.historyPos].c_str());
				} else {
					// Restore partial command and cursor pos.
					data->DeleteChars(0, data->BufTextLen);
					data->InsertChars(0, state.savedPartialCommand.c_str());
					data->CursorPos = state.savedCursor;
				}

			}
		}

		default:
			break;
	}
	return 0;
}

int printLines(const std::vector<UILine>& lines, const UIStyle &style, bool selectable) {
	int selectedLine = -1;

	const size_t lineCount = lines.size();
	ImGuiListClipper clipper;
	clipper.Begin(lineCount, ImGui::GetTextLineHeightWithSpacing());

	while (clipper.Step()) {
		for (int lid = clipper.DisplayStart; lid < clipper.DisplayEnd; ++lid) {
			const UILine& line = lines[lid];
			const size_t wordCount = line.words.size();

			if (line.type == UILine::EMPTY) {
				ImGui::TextUnformatted("");
				continue;
			}

			// Errors only have basic formatting.
			if (line.type == UILine::ISSUE) {
				ImGui::PushStyleColor(ImGuiCol_Text, style.errorColor);
				ImGui::TextUnformatted(line.fullText.c_str());
				ImGui::PopStyleColor();
				continue;
			}

			// Input/output lines are syntax highlighted.
			for (size_t wid = 0; wid < wordCount; ++wid) {
				// Pack words on the same line.
				if (wid != 0) {
					ImGui::SameLine(0, 0);
				}

				const UILine::UIWord& word = line.words[wid];
				ImGui::PushStyleColor(ImGuiCol_Text, style.wordColors[word.type]);

				if (word.type == Calculator::Word::OPERATOR && wid != 0) {
					ImGui::TextUnformatted(" ");
					ImGui::SameLine(0, 0);
				}
				if (selectable && wid == 0) {
					ImGui::PushID(int(lid));
					if (ImGui::Selectable(word.text.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
						selectedLine = lid;
					}
					ImGui::PopID();
				}
				else {
					ImGui::TextUnformatted(word.text.c_str());
				}
				if (word.type == Calculator::Word::OPERATOR) {

					ImGui::SameLine(0, 0);
					ImGui::TextUnformatted(" ");
				}
				ImGui::PopStyleColor();
			}
		}
	}
	clipper.End();
	return selectedLine;
}


void customizeStyle(UIStyle &tmpStyle) {
	// Preview basic style:
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, tmpStyle.backgroundColor);
	const float textHeight = ImGui::GetTextLineHeightWithSpacing();
	ImGui::PushFont(tmpStyle.consoleFont);

	ImGui::BeginChildFrame(ImGui::GetID("##Example style"), ImVec2(0, textHeight * 6.5f));

	static bool initLines = false;
	static std::vector<UILine> lines = {
		{UILine::Type::INPUT, ""},
		{UILine::Type::OUTPUT, ""},
		{UILine::Type::INPUT, ""},
		{UILine::Type::ISSUE, ""},
	};

	// Lazy init.
	if(!initLines){
		// Lines presenting all types of words.

		lines[0].words = {
			{"x", Calculator::Word::VARIABLE}, {"=", Calculator::Word::OPERATOR}, {"3.0", Calculator::Word::LITERAL},
			{"*", Calculator::Word::OPERATOR}, {"sin", Calculator::Word::FUNCTION},  {"(", Calculator::Word::SEPARATOR},
			{"1", Calculator::Word::LITERAL}, {"+", Calculator::Word::OPERATOR},  {"2.2", Calculator::Word::LITERAL},
			{"*", Calculator::Word::OPERATOR}, {"pi", Calculator::Word::VARIABLE},  {")", Calculator::Word::SEPARATOR}
		};
		lines[1].words = {
			{"=", Calculator::Word::OPERATOR}, {"2.995038", Calculator::Word::RESULT}
		};
		lines[2].words = {
			{"y_val", Calculator::Word::VARIABLE}, {"=", Calculator::Word::OPERATOR}, {"max", Calculator::Word::FUNCTION}, {"(", Calculator::Word::SEPARATOR},
			{"x", Calculator::Word::VARIABLE}, {"*", Calculator::Word::OPERATOR},  {"2.0", Calculator::Word::LITERAL},
			{",", Calculator::Word::SEPARATOR}, {"z_top", Calculator::Word::VARIABLE},  {")", Calculator::Word::SEPARATOR}
		};
		lines[3].words = {
			{"Error: unknown variable z_top", Calculator::Word::RESULT}
		};
		initLines =true;
	}

	printLines(lines, tmpStyle, false);

	ImGui::EndChild();

	ImGui::PopFont();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();

	ImGui::Dummy(ImVec2(0, 6));

	const uint32_t colorFlags = ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_DisplayHex;
	ImGui::PushItemWidth(120);
	ImGui::ColorEdit3("Background", &tmpStyle.backgroundColor.x, colorFlags);
	ImGui::SameLine(220);
	ImGui::ColorEdit3("Error",		&tmpStyle.errorColor.x, colorFlags);
	ImGui::ColorEdit3("Variables",	&tmpStyle.wordColors[Calculator::Word::VARIABLE].x, colorFlags);
	ImGui::SameLine(220);
	ImGui::ColorEdit3("Functions",	&tmpStyle.wordColors[Calculator::Word::FUNCTION].x, colorFlags);
	ImGui::ColorEdit3("Operators",	&tmpStyle.wordColors[Calculator::Word::OPERATOR].x, colorFlags);
	ImGui::SameLine(220);
	ImGui::ColorEdit3("Separators", &tmpStyle.wordColors[Calculator::Word::SEPARATOR].x, colorFlags);
	ImGui::ColorEdit3("Literals",	&tmpStyle.wordColors[Calculator::Word::LITERAL].x, colorFlags);
	ImGui::SameLine(220);
	ImGui::ColorEdit3("Result",		&tmpStyle.wordColors[Calculator::Word::RESULT].x, colorFlags);
	ImGui::PopItemWidth();
}

int main(int argc, char** argv){

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

	// Load settings.
	UIStyle style;
	style.loadFromFile(config.settingsPath);

	UIState state;
	GLFWwindow* window = createWindow(830, 620, style);

	if(!window){
		Log::Error() << "Unable to create window." << std::endl;
		return 1;
	}

	sr_gui_init();

	int winW, winH;

	const unsigned int winFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;

	char buffer[1024];
	memset(buffer, '\0', sizeof(buffer));

	UIStyle tmpStyle;

	Calculator calculator;
	/// TODO: save/restore calculator state (save all internal state + formatted output)


	bool shouldFocusTextField = true;
	while(!glfwWindowShouldClose(window)) {

		glfwPollEvents();

		// Screen resolution.
		glfwGetWindowSize(window, &winW, &winH);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		bool openPopup = false;

		if(ImGui::BeginMainMenuBar()){

			if(ImGui::BeginMenu("File")){

				if(ImGui::BeginMenu("Settings")){

					if(ImGui::MenuItem("Configure colors...")){
						tmpStyle = style;
						openPopup = true;
					}
					ImGui::Checkbox("Row major matrix display", &style.displayRowMajor);
					ImGui::EndMenu();
				}

				ImGui::Separator();

				if(ImGui::MenuItem("Clear log...")){
					const int result = sr_gui_ask_choice("Calco", "Are you sure you want to clear all logged operations?",
														 SR_GUI_MESSAGE_LEVEL_WARN, "Yes", "No", nullptr);
					if(result == SR_GUI_BUTTON0){
						state.lines.clear();
						state.commands.clear();
					}
				}
				if(ImGui::MenuItem("Clear memory...")){
					const int result = sr_gui_ask_choice("Calco", "Are you sure you want to clear?",
														 SR_GUI_MESSAGE_LEVEL_WARN, "Yes", "No", nullptr);
					if(result == SR_GUI_BUTTON0){
						calculator.clear();
					}
				}
				ImGui::Separator();
				if(ImGui::MenuItem("Quit")){
					glfwSetWindowShouldClose(window, GLFW_TRUE);
				}
				ImGui::EndMenu();
			}

			if(ImGui::BeginMenu("About")){
				ImGui::Text( "Calco - © Simon Rodriguez 2021" );
				ImGui::Text( "version 1.0.0" );
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (openPopup) {
			ImGui::OpenPopup("Color scheme");
		}

		const float menuBarHeight = ImGui::GetItemRectSize().y;
		const float heightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

		//ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		bool showCloseButton = true;
		if (ImGui::BeginPopupModal("Color scheme", &showCloseButton, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {

			// Example
			customizeStyle(tmpStyle);

			// Buttons
			if(ImGui::Button("Apply", ImVec2(120, 0))){ 
				style = tmpStyle;
				ImGui::CloseCurrentPopup(); 
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if(ImGui::Button("Revert", ImVec2(120, 0))){
				tmpStyle = style;
			}
			ImGui::SameLine();
			if(ImGui::Button("Default", ImVec2(120, 0))){
				tmpStyle.resetColors();
			}
			ImGui::EndPopup();
		}

		
		ImGui::SetNextWindowPos(ImVec2(0.0f, menuBarHeight));
		ImGui::SetNextWindowSize(ImVec2(float(winW), float(winH) - menuBarHeight));

		ImGui::PushStyleColor(ImGuiCol_WindowBg, style.backgroundColor);

		if(ImGui::Begin("CalcoMainWindow", nullptr, winFlags)){

			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -heightToReserve), false, ImGuiWindowFlags_HorizontalScrollbar);

			ImGui::PushFont(style.consoleFont);

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

			const int selectedLine = printLines(state.lines, style, true);
			if(selectedLine >= 0){
				// Store selected line in clipboard.
				const std::string& newContent = state.lines[selectedLine].fullText;
				ImGui::SetClipboardText(newContent.c_str());

				if(ImGui::IsMouseDoubleClicked(0)){
					// Register text to insert, will be done in the text field conitnuous callback.
					state.shouldInsert = true;
					state.textToInsert = newContent;
					shouldFocusTextField = true;
				}
			}

			ImGui::PopStyleVar();

			ImGui::PopFont();

			// Autoscroll to bottom.
			if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY()){
				ImGui::SetScrollHereY(1.0f);
			}
			ImGui::EndChild();
			ImGui::Separator();

			// Input line.
			const ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackAlways;

			ImGui::SetNextItemWidth(float(winW) - 2*ImGui::GetStyle().ItemSpacing.x);
			if(ImGui::InputText("##Input", buffer, 1024, inputTextFlags, &textCallback, &state)){
				std::string newLine(buffer);
				buffer[0] = '\0';

				if(!newLine.empty()){
					state.commands.push_back(newLine);
					state.historyPos = -1;
					state.savedPartialCommand = "";
					state.savedCursor = 0;

					Value result;
					Format format;
					std::vector<Calculator::Word> wordInfos;
					const bool success = calculator.evaluate(newLine, result, wordInfos, format);

					// Put a break before any input for clarity.
					state.lines.emplace_back(UILine::EMPTY, "");

					// Input line, with syntax highlighted words.
					UILine& line = state.lines.emplace_back(UILine::INPUT, newLine);
					for(const Calculator::Word& word : wordInfos){
						line.words.emplace_back(newLine.substr(word.location, word.size), word.type);
					}
					// Handle error that happened before syntax info generation.
					if(wordInfos.empty()){
						line.words.emplace_back(newLine, Calculator::Word::LITERAL);
					}

					// Output/error line
					if(!success){
						// Error line: passthrough the error message from the calculator.
						// The message can be multi-lines, split it.
						const std::vector<std::string> sublines = TextUtilities::split(result.str, "\n", false);
						for (const std::string& subline : sublines) {
							state.lines.emplace_back(UILine::ISSUE, subline);
						}
						
					} else if(result.type == Value::Type::STRING){
						// This is 'function definition' specific.
						state.lines.emplace_back( UILine::OUTPUT, result.str);
						state.lines.back().words.emplace_back(result.str, Calculator::Word::FUNCTION);
						state.lines.back().words.emplace_back(" defined", Calculator::Word::LITERAL);

					} else {
						// Used for copy/paste.
						const std::string internalStr = result.toString(Format::INTERNAL);

						// Build final display format.
						const Format tgtFormat = Format((format & Format::BASE_MASK) | (style.displayRowMajor ? Format::MAJOR_ROW_FLAG : Format::MAJOR_COL_FLAG));
						const std::string externalStr = result.toString(tgtFormat);

						// The message can be multi-lines, split it.
						const std::vector<std::string> sublines = TextUtilities::split(externalStr, "\n", false);
						bool first = true;
						for (const std::string& subline : sublines) {
							state.lines.emplace_back( UILine::OUTPUT, internalStr);
							if(first){
								state.lines.back().words.emplace_back("=", Calculator::Word::OPERATOR);
								state.lines.back().words.emplace_back(subline, Calculator::Word::RESULT);
								first = false;
							} else {
								state.lines.back().words.emplace_back(" ", Calculator::Word::OPERATOR);
								state.lines.back().words.emplace_back(subline, Calculator::Word::RESULT);
							}

						}

					}

				}
				shouldFocusTextField = true;
			}
			// Auto-focus on window apparition
			ImGui::SetItemDefaultFocus();
			if(shouldFocusTextField){
				ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
			}

		}
		ImGui::End();
		ImGui::PopStyleColor();

		shouldFocusTextField = false;

		// Render the interface.
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(window);
	}
	// Save settings.
	style.saveToFile(config.settingsPath);

	// Cleanup.
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	sr_gui_cleanup();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
