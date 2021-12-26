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

#include "font_data.h"

struct UIState {
	std::vector<std::string> lines;
	std::vector<std::string> commands;
	std::string savedPartialCommand;
	int savedCursor = 0;
	int historyPos = -1;
};

GLFWwindow* createWindow(int w, int h) {

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

	ImFontConfig font = ImFontConfig();
	font.FontData = (void*)(fontData);
	font.FontDataSize = size_fontData;
	font.SizePixels = 15.0f;
	// Font data is static
	font.FontDataOwnedByAtlas = false;
	ImGuiIO & io = ImGui::GetIO();
	io.Fonts->AddFont(&font);

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
	style.FrameRounding = 12;
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

	return window;
}

int textCallback(ImGuiInputTextCallbackData* data){
	UIState& state = *((UIState*)(data->UserData));
	switch (data->EventFlag) {
		case ImGuiInputTextFlags_CallbackHistory:
		{
			// Example of HISTORY
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

	GLFWwindow* window = createWindow(830, 620);

	if(!window){
		Log::Error() << "Unable to create window." << std::endl;
		return 1;
	}

	sr_gui_init();

	UIState state;

	int winW, winH;

	const unsigned int winFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;

	char buffer[1024];

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
				if(ImGui::MenuItem("Clear")){
					state.lines.clear();
					// Don't clear commands.
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

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
			const size_t lineCount = state.lines.size();
			for(size_t lid = 0; lid < lineCount; ++lid){
				ImGui::TextUnformatted(state.lines[lid].c_str());
			}
			ImGui::PopStyleVar();

			// Autoscroll to bottom.
			if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY()){
				ImGui::SetScrollHereY(1.0f);
			}
			ImGui::EndChild();
			ImGui::Separator();
			// Input line.

			bool reclaimFocus = false;
			const ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CharsNoBlank;

			ImGui::SetNextItemWidth(float(winW) - 2*ImGui::GetStyle().ItemSpacing.x);
			if(ImGui::InputText("##Input", buffer, 1024, inputTextFlags, &textCallback, &state)){
				std::string newLine(buffer);
				buffer[0] = '\0';
				if(!newLine.empty()){
					state.lines.push_back(newLine);
					state.commands.push_back(newLine);
					state.historyPos = -1;
					state.savedPartialCommand = "";
					state.savedCursor = 0;
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


		//sr_gui_show_message("Calco", "Invalid operation", SR_GUI_MESSAGE_LEVEL_ERROR);


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
