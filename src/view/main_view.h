#ifndef MAIN_VIEW
#define MAIN_VIEW

#include "../data_handler/data_handler.h"
#include "data_context.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
	#error This backend required SDL 2.0.17+ becuase of SDL_RenderGeometry() function
#endif

class MainView {
public:
	MainView(struct DataContext* ptrDataContext, DataHandler* ptrDataHandler);

	~MainView();

	void DisplayMain();

	void SearchBar();

	void DisplayLoginBtn(std::map<std::string, std::string> login,
						const char* loginDetails, float loginSecWidth,
						int idx, int numLogins);
	
	void DisplayLogins(std::vector<std::map<std::string, std::string>> logins);

	void DisplayLogin(float loginSecWidth);

	void EditLogin();

private:
	struct DataContext* ptrDataContext;
	DataHandler* ptrDataHandler;
	char searchBuff[DEFAULT_LOGINS_MAX_LENGTH] = "";
};

#endif