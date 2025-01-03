#include "UIElements.hpp"
#include "Grapher.hpp"

#include "core/Common.hpp"
#include "core/Strings.hpp"
#include "core/Calculator.hpp"
#include "core/Settings.hpp"

#include "core/system/Config.hpp"
#include "core/system/System.hpp"
#include "core/system/TextUtilities.hpp"

#if defined(__EMSCRIPTEN__)
    #include <emscripten/emscripten.h>
    #include <emscripten/html5.h>
	#define GL_GLEXT_PROTOTYPES
	#define EGL_EGLEXT_PROTOTYPES
	#define IMGUI_IMPL_OPENGL_ES3
#else
	#define GL_VERSION_3_2
	#include <gl3w/gl3w.h>
#endif

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <implot/implot.h>
#include <sr_gui/sr_gui.h>

#include <unordered_map>

#include "fonts/font_data_Inconsolata.h"
#include "fonts/font_data_Lato.h"

#ifdef _WIN32
// Avoid command prompt appearing on startup
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif

GLFWwindow* createWindow(int w, int h, UIStyle& uiStyle) {

	// Initialize glfw, which will create and setup an OpenGL context.
	if(!glfwInit()) {
		return NULL;
	}

#if defined(__EMSCRIPTEN__)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	const unsigned int ww = std::max( mode->width/2, w);
	const unsigned int hh = std::max( mode->height/2, h);

	GLFWwindow* window = glfwCreateWindow(ww, hh, "Calco", NULL, NULL);

	if(!window) {
		Log::Error() << "No window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);

#if !defined(__EMSCRIPTEN__)
	if(gl3wInit()) {
		return NULL;
	}
	if(!gl3wIsSupported(3, 2)) {
		return NULL;
	}
#endif

	glfwSwapInterval(1);

	ImGui::CreateContext();
	ImPlot::CreateContext();
	
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
	io.IniFilename = nullptr;
	uiStyle.textFont = io.Fonts->AddFont(&fontLato);
	uiStyle.consoleFont = io.Fonts->AddFont(&fontConsole);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(nullptr);

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ImVec2( 8, 8 );
	style.FramePadding = ImVec2( 10, 4 );
	style.CellPadding = ImVec2( 4, 2 );
	style.ItemSpacing = ImVec2( 10, 10 );
	style.ItemInnerSpacing = ImVec2( 4, 4 );
	style.GrabMinSize = 18;
	style.FrameBorderSize = 0;
	style.WindowBorderSize = 0;
	style.FrameRounding = 12;
	style.GrabRounding = 12;
	style.PopupBorderSize = 0;
	style.PopupRounding = 4;
	style.WindowRounding = 12;

	ImVec4* colors = style.Colors;
	colors[ImGuiCol_FrameBg] = ImVec4( 0.58f, 0.58f, 0.58f, 0.54f );
	colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.26f, 0.84f, 0.98f, 0.40f );
	colors[ImGuiCol_FrameBgActive] = ImVec4( 0.26f, 0.84f, 0.98f, 0.40f );
	colors[ImGuiCol_TitleBgActive] = ImVec4( 0.04f, 0.04f, 0.04f, 1.00f );
	colors[ImGuiCol_CheckMark] = ImVec4( 0.08f, 0.71f, 0.77f, 1.00f );
	colors[ImGuiCol_SliderGrab] = ImVec4( 0.05f, 0.61f, 0.73f, 1.00f );
	colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.03f, 0.69f, 0.82f, 1.00f );
	colors[ImGuiCol_Button] = ImVec4( 0.05f, 0.39f, 0.45f, 1.00f );
	colors[ImGuiCol_ButtonHovered] = ImVec4( 0.05f, 0.61f, 0.73f, 1.00f );
	colors[ImGuiCol_ButtonActive] = ImVec4( 0.03f, 0.69f, 0.82f, 1.00f );
	colors[ImGuiCol_Header] = ImVec4( 0.05f, 0.39f, 0.45f, 1.00f );
	colors[ImGuiCol_HeaderHovered] = ImVec4( 0.05f, 0.61f, 0.73f, 1.00f );
	colors[ImGuiCol_HeaderActive] = ImVec4( 0.03f, 0.69f, 0.82f, 1.00f );
	colors[ImGuiCol_SeparatorHovered] = ImVec4( 0.05f, 0.39f, 0.45f, 1.00f );
	colors[ImGuiCol_SeparatorActive] = ImVec4( 0.05f, 0.61f, 0.73f, 1.00f );
	colors[ImGuiCol_ResizeGrip] = ImVec4( 0.05f, 0.39f, 0.45f, 1.00f );
	colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.05f, 0.61f, 0.73f, 1.00f );
	colors[ImGuiCol_ResizeGripActive] = ImVec4( 0.03f, 0.69f, 0.82f, 1.00f );
	colors[ImGuiCol_Tab] = ImVec4( 0.05f, 0.39f, 0.45f, 1.00f );
	colors[ImGuiCol_TabHovered] = ImVec4( 0.03f, 0.69f, 0.82f, 1.00f );
	colors[ImGuiCol_TabActive] = ImVec4( 0.05f, 0.61f, 0.73f, 1.00f );
	colors[ImGuiCol_TabUnfocused] = ImVec4( 0.02f, 0.16f, 0.18f, 1.00f );
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4( 0.04f, 0.26f, 0.31f, 1.00f );
	colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.04f, 0.26f, 0.31f, 1.00f );
	colors[ImGuiCol_NavHighlight] = ImVec4( 0.05f, 0.61f, 0.73f, 1.00f );
	colors[ImGuiCol_PlotHistogram] = ImVec4( 0.05f, 0.61f, 0.73f, 1.00f );
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 0.05f, 0.61f, 0.73f, 1.00f );
	colors[ImGuiCol_PopupBg]           	  = ImVec4(0.15f, 0.15f, 0.15f, 0.94f);

	ImPlot::StyleColorsLight();
	ImPlotStyle& pstyle              = ImPlot::GetStyle();
	pstyle.PlotPadding				 = ImVec2(6,6);
	pstyle.LabelPadding				 = ImVec2(4,4);
	ImVec4* pcolors                  = pstyle.Colors;
	pcolors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
	pcolors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
	pcolors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
	pcolors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
	pcolors[ImPlotCol_ErrorBar]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	pcolors[ImPlotCol_FrameBg]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	pcolors[ImPlotCol_PlotBg]        = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
	pcolors[ImPlotCol_PlotBorder]    = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	pcolors[ImPlotCol_LegendBg]      = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
	pcolors[ImPlotCol_LegendBorder]  = ImVec4(0.80f, 0.81f, 0.85f, 1.00f);
	pcolors[ImPlotCol_LegendText]    = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	pcolors[ImPlotCol_TitleText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	pcolors[ImPlotCol_InlayText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	pcolors[ImPlotCol_AxisText]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	pcolors[ImPlotCol_AxisGrid]      = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	pcolors[ImPlotCol_AxisBgHovered] = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
	pcolors[ImPlotCol_AxisBgActive]  = ImVec4(0.92f, 0.92f, 0.95f, 0.75f);
	pcolors[ImPlotCol_Selection]     = ImVec4(1.00f, 0.65f, 0.00f, 1.00f);
	pcolors[ImPlotCol_Crosshairs]    = ImVec4(0.23f, 0.10f, 0.64f, 0.50f);
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
		case ImGuiInputTextFlags_CallbackEdit:
		{
			state.evaluatePartial = true;
			break;
		}
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
	clipper.Begin(int(lineCount), ImGui::GetTextLineHeightWithSpacing());

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

void loadStateFromFile(const std::string& path, UIState& state, Calculator& calculator) {
	std::ifstream file(path);
	if (!file.is_open()) {
		Log::Error() << "Unable to open state at \"" << path << "\"" << std::endl;
		return;
	}

	std::string elem;
	file >> elem;
	if (elem == "UISTATE") {
		state.loadFromStream(file);
		file >> elem;
	}
	if (elem == "CALCSTATE") {
		calculator.loadFromStream(file);
		file >> elem;
	}
	file >> elem;
	file.close();
}

void saveStateToFile(const std::string& path, const UIState& state, const Calculator& calculator) {
	std::ofstream file(path);
	if(!file.is_open()) {
		Log::Error() << "Unable to save state to \"" << path << "\"" << std::endl;
		return;
	}
	state.saveToStream(file);
	calculator.saveToStream(file);
	file.close();
}

#if defined(__EMSCRIPTEN__)


void mainLoop(void * function){
	std::function<void()>* processFrame = (std::function<void()>*)function;
	(*processFrame)();
}

void prepareFilesystem(){
	EM_ASM(
        FS.mkdir('/Calco');
        FS.mount(IDBFS, { autoPersist: true }, '/Calco');
        FS.syncfs(true, function (err) { });
    );
}

#endif

int main(int argc, char** argv){

#if defined(__EMSCRIPTEN__)
	prepareFilesystem();
#endif

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

	UIStyle style;
	GLFWwindow* window = createWindow(1280, 720, style);

	// Load settings after creating the window, as the current directory might have changed.
	style.loadFromFile(config.settingsPath);

	if(!window){
		Log::Error() << "Unable to create window." << std::endl;
		return 1;
	}

	sr_gui_init();

	const unsigned int winFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;

	char buffer[1024];
	memset(buffer, '\0', sizeof(buffer));

	UIStyle tmpStyle;
	UIState state;
	Grapher grapher;
	Calculator calculator;
	
	// Save/restore calculator state (save all internal state + formatted output)
	loadStateFromFile(config.historyPath, state, calculator);
	// Apply style.
	calculator.updateDocumentation(style.format);
	// Recreate graph definitions of save functions.
	for(const auto& func : calculator.functions()){
		FunctionGraph& graph = grapher.addOrUpdateFunction(func.first, func.second);
		graph.validate(calculator);

	}

	//
	bool shouldFocusTextField = true;
	bool showCompilationResult = false;
	bool failedCompilation = false;
	std::string compilationMsg = "";

#if defined(__EMSCRIPTEN__)
	std::function<void()> processFrame = [&](){
#else
	while(!glfwWindowShouldClose(window))
	{
#endif
		glfwWaitEventsTimeout(0.1);
		// Screen resolution.
		int winW, winH;
		glfwGetWindowSize(window, &winW, &winH);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		bool openPopup = false;
		bool updateDoc = false;

		// Menus and settings
		{

			if(ImGui::BeginMainMenuBar()){
				if(ImGui::BeginMenu("File")){

					if(ImGui::BeginMenu("Settings")){

						if(ImGui::MenuItem("Configure colors...")){
							tmpStyle = style;
							openPopup = true;
						}
						bool displayRowMajor = (style.format & Format::MAJOR_MASK) == Format::MAJOR_ROW_FLAG;
						if(ImGui::Checkbox("Row major matrix display", &displayRowMajor)){
							const Format majorFormat = displayRowMajor ? Format::MAJOR_ROW_FLAG : Format::MAJOR_COL_FLAG;
							style.format = Format((style.format & ~Format::MAJOR_MASK) | majorFormat);
							updateDoc = true;
						}
						ImGui::PushItemWidth(120);
						int base = (style.format & Format::BASE_MASK) >> 1;
						if(ImGui::Combo("Integer base", &base, "Binary\0Octal\0Hexadecimal\0Decimal\0")){
							const Format baseFormat = Format((base << 1) & Format::BASE_MASK);
							style.format = Format((style.format & ~Format::BASE_MASK) | baseFormat);
							updateDoc = true;
						}
						ImGui::PopItemWidth();
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
							grapher.clear();
						}
					}
				#if !defined(__EMSCRIPTEN__)
					ImGui::Separator();
					if(ImGui::MenuItem("Quit")){
						glfwSetWindowShouldClose(window, GLFW_TRUE);
					}
				#endif
					ImGui::EndMenu();
				}

				if(ImGui::BeginMenu("View")){
					ImGui::MenuItem("Functions", nullptr, &state.showFunctions, true);
					ImGui::MenuItem("Variables", nullptr, &state.showVariables, true);
					ImGui::MenuItem("Library", nullptr, &state.showLibrary, true);
					ImGui::Separator();
					ImGui::MenuItem("Grapher", nullptr, &state.showGrapher, true);
					ImGui::EndMenu();
				}

				if(ImGui::BeginMenu("About")){
					if(ImGui::MenuItem(versionMessage.c_str())){
						sr_gui_open_in_browser("https://github.com/kosua20/Calco");
					}
					if(ImGui::MenuItem("© Simon Rodriguez 2022")){
						sr_gui_open_in_browser("http://simonrodriguez.fr");
					}
					ImGui::Text("Size: %dx%d", winW, winH);
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}

			if (openPopup) {
				ImGui::OpenPopup("Color scheme");
			}
		}

		const float menuBarHeight = ImGui::GetItemRectSize().y;

		{
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
		}

		const float heightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		
		ImGui::SetNextWindowPos(ImVec2(0.0f, menuBarHeight));
		ImGui::SetNextWindowSize(ImVec2(float(winW), float(winH) - menuBarHeight));

		ImGui::PushStyleColor(ImGuiCol_WindowBg, style.backgroundColor);

		if(ImGui::Begin("CalcoMainWindow", nullptr, winFlags)){

			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -heightToReserve), false, ImGuiWindowFlags_HorizontalScrollbar);

			ImGui::PushFont(style.consoleFont);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

			// Print all lines.
			const int selectedLine = printLines(state.lines, style, true);
			if(selectedLine >= 0){
				const std::string& newContent = state.lines[selectedLine].fullText;

				if(ImGui::IsMouseDoubleClicked(0)){
					// Store selected line in clipboard.
					ImGui::SetClipboardText(newContent.c_str());
				} else {
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
			if(ImGui::IsKeyPressed(ImGuiKey_PageDown)){
				ImGui::SetScrollHereY(1.f);
			}

			ImGui::EndChild();
			ImGui::Separator();

			// Input line.
			const ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_CallbackEdit;

			ImGui::SetNextItemWidth(float(winW) - 2*ImGui::GetStyle().ItemSpacing.x);
			if(ImGui::InputText("##Input", buffer, 1024, inputTextFlags, &textCallback, &state)){
				std::string newLine(buffer);
				buffer[0] = '\0';

				if(!newLine.empty()){
					state.commands.push_back(newLine);
					state.historyPos = -1;
					state.savedPartialCommand = "";
					state.savedCursor = 0;
					compilationMsg = "";
					showCompilationResult = false;

					Value result;
					Format format = style.format;
					std::vector<Calculator::Word> wordInfos;
					const bool success = calculator.evaluate(newLine, result, wordInfos, format, false);

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

						const std::string& name = result.str;
						FunctionGraph& graph = grapher.addOrUpdateFunction(name, calculator.functions().at(name));
						graph.validate(calculator);
						// If the graph window is opened, mark the function as shown.
						graph.show = state.showGrapher && !graph.invalid;

					} else {
						
						// Used for copy/paste.
						const std::string internalStr = result.toString(Format::INTERNAL);

						// Build final display properly formatted.
						const std::string externalStr = result.toString(format);

						// The message can be multi-lines, split it.
						const std::vector<std::string> sublines = TextUtilities::split(externalStr, "\n", false);
						bool first = true;
						
						for (const std::string& subline : sublines) {
							state.lines.emplace_back( UILine::OUTPUT, internalStr);
							state.lines.back().words.emplace_back(first ? "=" : " ", Calculator::Word::OPERATOR);
							state.lines.back().words.emplace_back(subline, Calculator::Word::RESULT);
							first = false;
						}

					}
					glfwPostEmptyEvent();
				}
				shouldFocusTextField = true;
			}

			// Auto-focus on window apparition
			ImGui::SetItemDefaultFocus();
			if(shouldFocusTextField){
				ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
			}
			if(!ImGui::IsItemActive()){
				showCompilationResult = false;
			}
			if(ImGui::IsItemActivated()){
				showCompilationResult = true;
			}

			// Evaluate current typing if edited.
			if(state.evaluatePartial){
				state.evaluatePartial = false;
				std::string newLine(buffer);

				if(!newLine.empty()){
					Value result;
					Format format = style.format;
					std::vector<Calculator::Word> infos;
					const bool success = calculator.evaluate(newLine, result, infos, format, true);
					compilationMsg = "";
					if(!success){
						showCompilationResult = true;
						compilationMsg = result.str;
						failedCompilation = true;
					} else if(result.type != Value::Type::STRING){
						showCompilationResult = true;
						compilationMsg = result.toString(format);
						failedCompilation = false;
					}
				}
			}


		}
		const float mainWindowHeight = ImGui::GetWindowHeight();
		ImGui::End();
		ImGui::PopStyleColor();

		if(showCompilationResult && !compilationMsg.empty()){
			ImGui::SetNextWindowPos(ImVec2(0, mainWindowHeight + menuBarHeight - heightToReserve), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
			if(ImGui::Begin("Result display", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)){

				ImGui::PushFont(style.consoleFont);
				if(failedCompilation){
					ImGui::PushStyleColor(ImGuiCol_Text, style.errorColor);
				}
				ImGui::TextUnformatted(compilationMsg.c_str());
				if(failedCompilation){
					ImGui::PopStyleColor();
				}
				ImGui::PopFont();
			}
			ImGui::End();
		}


		shouldFocusTextField = false;

		// Documentation.
		{
			const int tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
			const int panelFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
			const int selectFlags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;
			const float baseHeight = ImGui::GetTextLineHeight();

			const ImVec2 outerSize(400, 300);
			const ImVec2 outerSizeFunc(outerSize.x + 100, outerSize.y);

			if(state.showFunctions){
				ImGui::SetNextWindowSize(outerSizeFunc, ImGuiCond_Once);
				ImGui::SetNextWindowSizeConstraints(ImVec2(outerSizeFunc.x, 0), ImVec2(outerSizeFunc.x, FLT_MAX));
				if (ImGui::Begin("Functions", &state.showFunctions, panelFlags)) {

					const ImVec2 innerSize(ImGui::GetWindowSize().x - 25, 0);

					if(ImGui::BeginTable("##FunctionsTable", 2, tableFlags, innerSize)){
						ImGui::TableSetupColumn("Name");
						ImGui::TableSetupColumn("Expression");
						ImGui::TableHeadersRow();

						for (const auto& func : calculator.functions()) {
							ImGui::TableNextColumn();

							if (ImGui::Selectable(func.second.name.c_str(), false, selectFlags, ImVec2(0, baseHeight))){
								// Register text to insert, will be done in the text field continuous callback.
								state.shouldInsert = true;
								state.textToInsert = func.second.name;
								shouldFocusTextField = true;
							}
							ImGui::TableNextColumn();
							ImGui::PushTextWrapPos(innerSize.x);
							ImGui::TextUnformatted(func.second.expression.c_str());
							ImGui::PopTextWrapPos();
						}
						ImGui::EndTable();
					}
				}
				ImGui::End();
			}

			if(state.showVariables){
				ImGui::SetNextWindowSize(outerSize, ImGuiCond_Once);
				ImGui::SetNextWindowSizeConstraints(ImVec2(outerSize.x, 0), ImVec2(outerSize.x, FLT_MAX));
				if (ImGui::Begin("Variables", &state.showVariables, panelFlags)) {

					// Options for display:
					bool displayRowMajor = (style.format & Format::MAJOR_MASK) == Format::MAJOR_ROW_FLAG;
					if(ImGui::Checkbox("Row major matrices", &displayRowMajor)){
						const Format majorFormat = displayRowMajor ? Format::MAJOR_ROW_FLAG : Format::MAJOR_COL_FLAG;
						style.format = Format((style.format & ~Format::MAJOR_MASK) | majorFormat);
						updateDoc = true;
					}
					ImGui::SameLine();
					ImGui::PushItemWidth(120);
					int base = (style.format & Format::BASE_MASK) >> 1;
					if(ImGui::Combo("Integer base", &base, "Binary\0Octal\0Hexadecimal\0Decimal\0")){
						const Format baseFormat = Format((base << 1) & Format::BASE_MASK);
						style.format = Format((style.format & ~Format::BASE_MASK) | baseFormat);
						updateDoc = true;
					}
					ImGui::PopItemWidth();

					const ImVec2 innerSize(ImGui::GetWindowSize().x - 25, 0);
					if(ImGui::BeginTable("##VariablesTable", 2, tableFlags, innerSize)){
						ImGui::TableSetupColumn("Name");
						ImGui::TableSetupColumn("Value");
						ImGui::TableHeadersRow();

						for (const auto& var : calculator.variables()) {
							ImGui::TableNextColumn();

							const float rowHeight = var.second.count * baseHeight;
							if (ImGui::Selectable(var.first.c_str(), false, selectFlags, ImVec2(0, rowHeight))){
								// Register text to insert, will be done in the text field conitnuous callback.
								state.shouldInsert = true;
								state.textToInsert = var.first;
								shouldFocusTextField = true;
							}

							ImGui::TableNextColumn();
							ImGui::TextUnformatted(var.second.value.c_str());
						}
						ImGui::EndTable();
					}
				}
				ImGui::End();
			}

			if(state.showLibrary){
				ImGui::SetNextWindowSize(outerSize, ImGuiCond_Once);
				ImGui::SetNextWindowSizeConstraints(ImVec2(outerSize.x, 0), ImVec2(outerSize.x, FLT_MAX));
				if(ImGui::Begin("Library", &state.showLibrary, panelFlags)) {

					const ImVec2 innerSize(ImGui::GetWindowSize().x - 25, 0);

					if(ImGui::BeginTabBar("Library elements")) {

						if(ImGui::BeginTabItem("Functions")) {

							if(ImGui::BeginTable("##LibTableFunctions", 2, tableFlags, innerSize)){
								ImGui::TableSetupColumn("Name");
								ImGui::TableSetupColumn("Parameters");
								ImGui::TableHeadersRow();
								for (const auto& func : calculator.stdlib()) {
									ImGui::TableNextColumn();

									if (ImGui::Selectable(func.second.name.c_str(), false, selectFlags, ImVec2(0, baseHeight))){
										// Register text to insert, will be done in the text field conitnuous callback.
										state.shouldInsert = true;
										state.textToInsert = func.second.name + "()";
										shouldFocusTextField = true;
									}
									ImGui::TableNextColumn();
									ImGui::TextUnformatted(func.second.expression.c_str());
								}
								ImGui::EndTable();
							}

							ImGui::EndTabItem();
						}

						if(ImGui::BeginTabItem("Constants")) {

							if(ImGui::BeginTable("##ConstantTable", 2, tableFlags, innerSize)){
								ImGui::TableSetupColumn("Name");
								ImGui::TableSetupColumn("Value");
								ImGui::TableHeadersRow();

								for (const auto& var : calculator.constants()) {
									ImGui::TableNextColumn();

									const float rowHeight = var.second.count * baseHeight;
									if (ImGui::Selectable(var.first.c_str(), false, selectFlags, ImVec2(0, rowHeight))){
										state.shouldInsert = true;
										state.textToInsert = var.first;
										shouldFocusTextField = true;
									}

									ImGui::TableNextColumn();
									ImGui::TextUnformatted(var.second.value.c_str());
								}
								ImGui::EndTable();
							}

							ImGui::EndTabItem();
						}
						ImGui::EndTabBar();
					}


				}
				ImGui::End();
			}

			if(updateDoc){
				calculator.updateDocumentation(style.format);
			}

		}

		// Graphing
		if(state.showGrapher){

			ImGui::SetNextWindowPos(ImVec2(0.0f, menuBarHeight));
			ImGui::SetNextWindowSize(ImVec2(float(winW), float(winH)- menuBarHeight - heightToReserve ));
			if(ImGui::Begin("Grapher", &state.showGrapher)){

				const bool refresh = grapher.display(calculator);
				if(refresh){
					glfwPostEmptyEvent();
				}
			}
			ImGui::End();
		}
		
		// Render the interface.
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(window);
	
#if defined(__EMSCRIPTEN__)
	};
	emscripten_set_main_loop_arg(mainLoop, &processFrame, 0, true);
#else
	}
#endif

	// Save settings.
	style.saveToFile(config.settingsPath);

	// Save internal state.
	saveStateToFile(config.historyPath, state, calculator);

	// Cleanup.
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	sr_gui_cleanup();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
