#include "core/Common.hpp"
#include "core/Strings.hpp"
#include "core/Calculator.hpp"

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

struct UIStyle {

	enum Color {
		DEFAULT = 0, OUTPUT, ERROR, LITERAL, VARIABLE, FUNCTION, OPERATOR, SEPARATOR, COUNT
	};

	ImFont* consoleFont = nullptr;
	ImFont* textFont = nullptr;
	ImVec4 colors[UIStyle::Color::COUNT];
};

struct UILine {

	struct UIWord {

		UIWord(const std::string& _text, UIStyle::Color _color, bool _pad = false) : text(_text), color(_color), pad(_pad) {}

		std::string text;
		UIStyle::Color color;
		bool pad;
	};

	std::vector<UIWord> words;

};

struct UIState {
	std::vector<UILine> lines;
	std::vector<std::string> commands;
	std::string savedPartialCommand;
	int savedCursor = 0;
	int historyPos = -1;
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

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_FrameBg]                = ImVec4(0.58f, 0.58f, 0.58f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.84f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.84f, 0.98f, 0.40f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.08f, 0.71f, 0.77f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.05f, 0.61f, 0.73f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.03f, 0.69f, 0.82f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.39f, 0.45f, 1.00f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.05f, 0.61f, 0.73f, 1.00f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.03f, 0.69f, 0.82f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.05f, 0.39f, 0.45f, 1.00f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.05f, 0.61f, 0.73f, 1.00f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.03f, 0.69f, 0.82f, 1.00f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.05f, 0.39f, 0.45f, 1.00f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.05f, 0.61f, 0.73f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.05f, 0.39f, 0.45f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.05f, 0.61f, 0.73f, 1.00f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.03f, 0.69f, 0.82f, 1.00f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.05f, 0.39f, 0.45f, 1.00f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.03f, 0.69f, 0.82f, 1.00f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.05f, 0.61f, 0.73f, 1.00f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.02f, 0.16f, 0.18f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.04f, 0.26f, 0.31f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.04f, 0.26f, 0.31f, 1.00f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.05f, 0.61f, 0.73f, 1.00f);
	colors[ImGuiCol_PopupBg]           		= ImVec4(0.15f, 0.15f, 0.15f, 0.94f);

	/// TODO: custom styles
	/// TODO: add background color
	uiStyle.colors[UIStyle::Color::DEFAULT] = colors[ImGuiCol_Text];
	uiStyle.colors[UIStyle::Color::OUTPUT] = ImVec4(0.9f, 0.95f, 1.0f, 1.0f);
	uiStyle.colors[UIStyle::Color::ERROR] = ImVec4(0.8f, 0.2f, 0.1f, 1.0f);
	uiStyle.colors[UIStyle::Color::LITERAL] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	uiStyle.colors[UIStyle::Color::VARIABLE] = ImVec4(0.258824f, 0.545098f, 0.000000f, 1.0f);
	uiStyle.colors[UIStyle::Color::FUNCTION] = ImVec4(0.886275f, 0.035294f, 0.113725f, 1.0f);
	uiStyle.colors[UIStyle::Color::OPERATOR] = ImVec4(0.713726f, 0.560784f, 0.000000f, 1.0f);
	uiStyle.colors[UIStyle::Color::SEPARATOR] = ImVec4(0.603922f, 0.415686f, 0.600000f, 1.0f);
	return window;
}

int textCallback(ImGuiInputTextCallbackData* data){
	UIState& state = *((UIState*)(data->UserData));
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


int main(int, char** ){

	UIStyle style;
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
	Calculator calculator;

	/// TODO: save/restore calculator state (save all internal state + formatted output)

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

		if(ImGui::BeginMainMenuBar()){

			if(ImGui::BeginMenu("File")){

				/// TODO: extra settings for styling

				if(ImGui::MenuItem("Clear...")){
					const int result = sr_gui_ask_choice("Calco", "Are you sure you want to clear?",
														 SR_GUI_MESSAGE_LEVEL_WARN, "Yes", "No", "");
					if(result == SR_GUI_BUTTON0){
						state.lines.clear();
						// Don't clear commands.
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
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		const int menuBarHeight = ImGui::GetItemRectSize().y;
		ImGui::SetNextWindowPos(ImVec2(0.0f, menuBarHeight));
		ImGui::SetNextWindowSize(ImVec2(float(winW), float(winH - menuBarHeight)));

		if(ImGui::Begin("CalcoMainWindow", nullptr, winFlags)){
			const float heightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -heightToReserve), false, ImGuiWindowFlags_HorizontalScrollbar);

			/// TODO: make lines selectable and copy/pastable
			/// TODO: double click to paste to input field

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
			ImGui::PushFont(style.consoleFont);

			const size_t lineCount = state.lines.size();
			for(size_t lid = 0; lid < lineCount; ++lid){

				/// TODO: group each input+output with a tighter spacing
				const auto& words = state.lines[lid].words;
				bool first = true;
				for(const auto& word : words){
					if(!first){
						ImGui::SameLine(0,0);
					}
					first = false;
					ImGui::PushStyleColor(ImGuiCol_Text, style.colors[word.color]);
					if(word.pad){
						ImGui::TextUnformatted(" ");
						ImGui::SameLine(0,0);
					}
					ImGui::TextUnformatted(word.text.c_str());
					if(word.pad){
						/// TODO: support unary operator without space after if after another operator or beginning of line
						ImGui::SameLine(0,0);
						ImGui::TextUnformatted(" ");
					}
					ImGui::PopStyleColor();
				}

			}

			ImGui::PopFont();
			ImGui::PopStyleVar();

			// Autoscroll to bottom.
			if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY()){
				ImGui::SetScrollHereY(1.0f);
			}
			ImGui::EndChild();
			ImGui::Separator();
			// Input line.

			bool reclaimFocus = false;
			const ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory;

			ImGui::SetNextItemWidth(float(winW) - 2*ImGui::GetStyle().ItemSpacing.x);
			if(ImGui::InputText("##Input", buffer, 1024, inputTextFlags, &textCallback, &state)){
				std::string newLine(buffer);
				buffer[0] = '\0';
				if(!newLine.empty()){
					state.commands.push_back(newLine);
					state.historyPos = -1;
					state.savedPartialCommand = "";
					state.savedCursor = 0;

					std::string resultLine;
					std::vector<Calculator::SemanticInfo> syntaxInfos;
					const bool success = calculator.evaluate(newLine, resultLine, syntaxInfos);

					UILine& line = state.lines.emplace_back();

					static const std::unordered_map<Calculator::SemanticInfo::Type, UIStyle::Color> syntaxToColor = {
						{ Calculator::SemanticInfo::Type::LITERAL, UIStyle::Color::LITERAL},
						{ Calculator::SemanticInfo::Type::VARIABLE, UIStyle::Color::VARIABLE},
						{ Calculator::SemanticInfo::Type::FUNCTION, UIStyle::Color::FUNCTION},
						{ Calculator::SemanticInfo::Type::OPERATOR, UIStyle::Color::OPERATOR},
						{ Calculator::SemanticInfo::Type::SEPARATOR, UIStyle::Color::SEPARATOR}
					};

					for(const auto& infos : syntaxInfos){
						line.words.emplace_back(newLine.substr(infos.location, infos.size), syntaxToColor.at(infos.type), infos.type == Calculator::SemanticInfo::Type::OPERATOR);
					}
					state.lines.emplace_back();
					state.lines.back().words = { { resultLine, success ? UIStyle::Color::OUTPUT : UIStyle::Color::ERROR } };
				}
				reclaimFocus = true;
			}

			// Auto-focus on window apparition
			ImGui::SetItemDefaultFocus();
			if(reclaimFocus){
				ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
			}

		}
		ImGui::End();

		// Render the interface.
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(window);
	}

	// Cleanup.

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	sr_gui_cleanup();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
