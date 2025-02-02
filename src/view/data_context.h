#ifndef DATA_CONTEXT
#define DATA_CONTEXT

#include <SDL2/SDL.h>
#include <map>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_filedialog.h"

enum View {
	MASTER_LOGIN_VIEW,
	MAIN_VIEW,
	ADD_LOGIN_VIEW,
	TRASH_VIEW
};

struct DataContext {
	View view;
};

#endif