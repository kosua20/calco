

#include "sr_gui.h"
#include "sr_gui_internal.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <emscripten.h>

void sr_gui_init() {
	// Nothing.
}

void sr_gui_cleanup() {
	// Nothing.
}

void sr_gui_show_message(const char* title, const char* message, int level) {
	char* header = "Info";
	if(level == SR_GUI_MESSAGE_LEVEL_ERROR) {
		header = "Error";
	} else if(level == SR_GUI_MESSAGE_LEVEL_WARN) {
		header = "Warning";
	}
	char buffer[SR_GUI_MAX_STR_SIZE];
	sprintf(buffer, "%s : %s\n %s", header, title ? title : "", message ? message : "");
	
	EM_ASM(alert(UTF8ToString($0)), buffer);
}

void sr_gui_show_notification(const char* title, const char* message) {
	char buffer[SR_GUI_MAX_STR_SIZE];
	sprintf(buffer, "%s : %s", title ? title : "Notification", message ? message : "");
	EM_ASM(alert(UTF8ToString($0)), buffer);
}

int sr_gui_ask_directory(const char* title, const char* startDir, char** outPath) {
	if(!outPath){
		return SR_GUI_CANCELLED;
	}
	*outPath = NULL;
	// (void)startDir;

	// fprintf(stdout, "--- Directory selection --- %s\n", title ? title : "");
	// fprintf(stdout, "Type the absolute path to a directory below and press enter to validate, or submit an empty line to cancel.\n");
	// char buffer[SR_GUI_MAX_STR_SIZE];
	// const int size = _sr_gui_query_line_from_cin(buffer, SR_GUI_MAX_STR_SIZE);
	// if(size == 0) {
	// 	return SR_GUI_CANCELLED;
	// }
	// // Copy to result, allocating enough space.
	// *outPath = (char*)SR_GUI_MALLOC(sizeof(char) * (size + 1));
	// if(*outPath == NULL) {
	// 	return SR_GUI_CANCELLED;
	// }
	// SR_GUI_MEMCPY(*outPath, buffer, sizeof(char) * (size + 1));
	return SR_GUI_VALIDATED;
}

int sr_gui_ask_load_files(const char* title, const char* startDir, const char* exts, char*** outPaths, int* outCount) {
	if(!outCount || !outPaths){
		return SR_GUI_CANCELLED;
	}
	*outCount = 0;
	*outPaths = NULL;
	(void)startDir;

	// fprintf(stdout, "--- File(s) selection --- %s\n", title ? title : "");
	// if(exts && strlen(exts) > 0) {
	// 	fprintf(stdout, "Allowed extensions: %s\n", exts);
	// }
	// fprintf(stdout, "Type the absolute path to a file on disk below and press enter. You can then input another path, or press enter again to finish. Submit an empty line to cancel\n");

	// char* tempPaths[SR_GUI_MAX_STR_SIZE];
	// int size   = 0;
	// int pathId = 0;
	// char buffer[SR_GUI_MAX_STR_SIZE];
	// while((size = _sr_gui_query_line_from_cin(buffer, SR_GUI_MAX_STR_SIZE)) != 0) {
	// 	// Copy the buffer.
	// 	tempPaths[pathId] = (char*)SR_GUI_MALLOC(sizeof(char) * (size + 1));
	// 	if(tempPaths[pathId] == NULL) {
	// 		continue;
	// 	}
	// 	SR_GUI_MEMCPY(tempPaths[pathId], buffer, sizeof(char) * (size + 1));

	// 	++pathId;
	// 	if(pathId >= SR_GUI_MAX_STR_SIZE) {
	// 		break;
	// 	}
	// }

	// *outCount = pathId;
	// if(pathId == 0) {
	// 	// No tempPaths allocation happened.
	// 	return SR_GUI_CANCELLED;
	// }

	// *outPaths = (char**)SR_GUI_MALLOC(sizeof(char*) * pathId);
	// if(*outPaths == NULL) {
	// 	// Free tempPaths string.
	// 	for(int i = 0; i < pathId; ++i) {
	// 		SR_GUI_FREE(tempPaths[i]);
	// 	}
	// 	return SR_GUI_CANCELLED;
	// }
	// SR_GUI_MEMCPY(*outPaths, tempPaths, sizeof(char*) * pathId);
	return SR_GUI_VALIDATED;
}


int sr_gui_ask_load_file(const char* title, const char* startDir, const char* exts, char** outPath) {
	if(!outPath){
		return SR_GUI_CANCELLED;
	}
	*outPath = NULL;
	// (void)startDir;

	// fprintf(stdout, "--- File selection --- %s\n", title ? title : "");
	// if(exts && strlen(exts) > 0) {
	// 	fprintf(stdout, "Allowed extensions: %s\n", exts);
	// }
	// fprintf(stdout, "Type the absolute path to a file on disk below and press enter to finish. Submit an empty line to cancel\n");

	// char buffer[SR_GUI_MAX_STR_SIZE];
	// const int size = _sr_gui_query_line_from_cin(buffer, SR_GUI_MAX_STR_SIZE);
	// if(size == 0) {
	// 	return SR_GUI_CANCELLED;
	// }
	// // Copy to result, allocating enough space.
	// *outPath = (char*)SR_GUI_MALLOC(sizeof(char) * (size + 1));
	// if(*outPath == NULL) {
	// 	return SR_GUI_CANCELLED;
	// }
	// SR_GUI_MEMCPY(*outPath, buffer, sizeof(char) * (size + 1));
	
	return SR_GUI_VALIDATED;
}

int sr_gui_ask_save_file(const char* title, const char* startDir, const char* exts, char** outPath) {
	if(!outPath){
		return SR_GUI_CANCELLED;
	}
	*outPath = NULL;
	(void)startDir;

	// fprintf(stdout, "--- Output selection --- %s\n", title ? title : "");
	// if(exts && strlen(exts) > 0) {
	// 	fprintf(stdout, "Allowed extensions: %s\n", exts);
	// }
	// fprintf(stdout, "Type the absolute path to an output file below and press enter to validate, or submit an empty to cancel.\n");

	// char buffer[SR_GUI_MAX_STR_SIZE];
	// const int size = _sr_gui_query_line_from_cin(buffer, SR_GUI_MAX_STR_SIZE);
	// if(size == 0) {
	// 	return SR_GUI_CANCELLED;
	// }
	// // Copy to result, allocating enough space.
	// *outPath = (char*)SR_GUI_MALLOC(sizeof(char) * (size + 1));
	// if(*outPath == NULL) {
	// 	return SR_GUI_CANCELLED;
	// }
	// SR_GUI_MEMCPY(*outPath, buffer, sizeof(char) * (size + 1));

	return SR_GUI_VALIDATED;
}

int sr_gui_ask_choice(const char* title, const char* message, int level, const char* button0, const char* button1, const char* button2) {
	char* header = "Info";
	if(level == SR_GUI_MESSAGE_LEVEL_ERROR) {
		header = "Error";
	} else if(level == SR_GUI_MESSAGE_LEVEL_WARN) {
		header = "Warning";
	}
	
	char buffer[SR_GUI_MAX_STR_SIZE];
	sprintf(buffer, "%s : %s\n%s", header, title ? title : "", message ? message : "");
	
	int outInt = EM_ASM_INT({
		return confirm(UTF8ToString($0));
	}, buffer);

	return outInt ? SR_GUI_BUTTON0 : SR_GUI_CANCELLED;
}

int sr_gui_ask_string(const char* title, const char* message, char** result) {
	if(!result){
		return SR_GUI_CANCELLED;
	}
	*result = NULL;
	char buffer[SR_GUI_MAX_STR_SIZE];
	sprintf(buffer, "%s: %s", title ? title : "", message ? message : "");
	
	char *outStr = (char*)EM_ASM_PTR({
		return stringToNewUTF8(prompt(UTF8ToString($0)))
	}, buffer);
	const int size = outStr == NULL ? 0 : strlen(outStr);
	if(size == 0){
		return SR_GUI_CANCELLED;
	}
	*result = (char*)SR_GUI_MALLOC(sizeof(char) * (size + 1));
	if(*result == NULL) {
		return SR_GUI_CANCELLED;
	}
	SR_GUI_MEMCPY(*result, outStr, sizeof(char) * (size + 1));
	free(outStr);
	return SR_GUI_VALIDATED;
}

int sr_gui_ask_color(unsigned char color[3]) {
	if(!color){
		return SR_GUI_CANCELLED;
	}
// 	fprintf(stdout, "--- Color selection\n");
// 	fprintf(stdout, "Current RGB color is: %d %d %d\n", (int)color[0], (int)color[1], (int)color[2]);
// 	fprintf(stdout, "Type the three values separated by space, and press enter to validate, or submit an empty line to cancel.\n");
// 	char buffer[64];
// 	const int size = _sr_gui_query_line_from_cin(buffer, 64);
// 	if(size == 0) {
// 		return SR_GUI_CANCELLED;
// 	}
// 	int rgb[3];
// #ifdef _WIN32
// 	int res = sscanf_s(buffer, "%d %d %d", &rgb[0], &rgb[1], &rgb[2]);
// #else
// 	int res = sscanf(buffer, "%d %d %d", &rgb[0], &rgb[1], &rgb[2]);
// #endif
// 	if(res < 3 || res == EOF) {
// 		return SR_GUI_CANCELLED;
// 	}
// 	for(int i = 0; i < 3; ++i) {
// 		color[i] = (unsigned char)(fmin(fmax(rgb[i], 0), 255));
// 	}
	return SR_GUI_VALIDATED;
}

int sr_gui_open_in_explorer(const char* path){
	//fprintf(stdout, "* Path: %s\n", path ? path : "");
	//fprintf(stdout, "Press enter to continue...");
	//_sr_gui_absorb_line_from_cin();
	return SR_GUI_VALIDATED;
}

int sr_gui_open_in_default_app(const char* path){
	//fprintf(stdout, "* Path: %s\n", path ? path : "");
	//fprintf(stdout, "Press enter to continue...");
	//_sr_gui_absorb_line_from_cin();
	return SR_GUI_VALIDATED;
}

int sr_gui_open_in_browser(const char* url){
	//fprintf(stdout, "* URL: %s\n", url ? url : "");
	//fprintf(stdout, "Press enter to continue...");
	//_sr_gui_absorb_line_from_cin();
	EM_ASM( window.open(UTF8ToString($0), '_blank'), url);
	return SR_GUI_VALIDATED;
}

int sr_gui_get_app_data_path(char** outPath) {
	if(!outPath){
		return SR_GUI_CANCELLED;
	}
	*outPath = (char*)SR_GUI_MALLOC(sizeof(char) * 2);
	if(*outPath == NULL) {
		return SR_GUI_CANCELLED;
	}
	(*outPath)[0] = '/';
	(*outPath)[1] = '\0';
	return SR_GUI_VALIDATED;
}
