#ifndef ADD_LOGIN_VIEW
#define ADD_LOGIN_VIEW

#include "../data_handler/data_handler.h"
#include "data_context.h"

class AddLoginView {
public:
	AddLoginView(struct DataContext* ptrDataContext, DataHandler* ptrDataHandler);

	~AddLoginView();

	void DisplayAddLogin();

private:
	struct DataContext* ptrDataContext;
	DataHandler* ptrDataHandler;
};

#endif