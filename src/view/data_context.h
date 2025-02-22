#ifndef DATA_CONTEXT
#define DATA_CONTEXT

#include <SDL2/SDL.h>
#include <map>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_filedialog.h"

#define DEFAULT_LOGINS_MAX_LENGTH (50)
#define WEBSITE_MAX_LENGTH (128)
#define NOTES_MAX_LENGTH (256)

enum View {
	MASTER_LOGIN_DISPLAY_VIEW,
	MAIN_DISPLAY_VIEW,
	ADD_LOGIN_DISPLAY_VIEW,
	TRASH_DISPLAY_VIEW
};

enum SubView {
	LOGIN_DISPLAY_VIEW,
	EDIT_LOGIN_DISPLAY_VIEW,
	NO_DISPLAY_VIEW
};

struct DataContext {
	View view = MASTER_LOGIN_DISPLAY_VIEW;
	SubView subView = NO_DISPLAY_VIEW;
	std::map<std::string, std::string> selectedLogin;
	bool isInitialEdit = false;
	int selectedLoginIdx = -1;
	char accountBuff[DEFAULT_LOGINS_MAX_LENGTH] = "";
	char typeBuff[DEFAULT_LOGINS_MAX_LENGTH] = "";
	char usernameBuff[DEFAULT_LOGINS_MAX_LENGTH] = "";
	char emailBuff[DEFAULT_LOGINS_MAX_LENGTH] = "";
	char passwordBuff[DEFAULT_LOGINS_MAX_LENGTH] = "";
	char websiteBuff[WEBSITE_MAX_LENGTH] = "";
	char notesBuff[NOTES_MAX_LENGTH] = "";
	char userBuff[DEFAULT_LOGINS_MAX_LENGTH] = "";
};

#endif