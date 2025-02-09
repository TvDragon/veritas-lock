#ifndef MENU_BAR
#define MENU_BAR

#include "../data_handler/data_handler.h"
#include "../data_handler/json_handler.h"
#include "data_context.h"

class MenuBar {
public:
	MenuBar(struct DataContext* ptrDataContext, DataHandler* ptrDataHandler);

	~MenuBar();

	void DisplayMenuBar();

private:
	struct DataContext* ptrDataContext;
	DataHandler* ptrDataHandler;
	bool m_fileDialogOpen = false;
	ImFileDialogInfo m_fileDialogInfo;
	std::vector<std::string> errorLogins;
	std::vector<std::string> failedReadLogins;
};

#endif