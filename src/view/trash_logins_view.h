#ifndef TRASH_LOGINS_VIEW
#define TRASH_LOGINS_VIEW

#include "../data_handler/data_handler.h"
#include "data_context.h"

class TrashLoginsView {
public:
	TrashLoginsView(struct DataContext* ptrDatacontext, DataHandler* ptrDataHandler);

	~TrashLoginsView();

	void DisplayTrashLoginsView();

private:
	struct DataContext* ptrDataContext;
	DataHandler* ptrDataHandler;
	int selectedRow = -1;
};

#endif