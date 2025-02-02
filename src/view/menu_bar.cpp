#include "menu_bar.h"

MenuBar::MenuBar(struct DataContext* ptrDataContext, DataHandler* ptrDataHandler)
	:ptrDataContext(ptrDataContext)
	, ptrDataHandler(ptrDataHandler)
{

}

MenuBar::~MenuBar() {
	errorLogins.clear();
	failedReadLogins.clear();
}

void MenuBar::DisplayMenuBar() {

}