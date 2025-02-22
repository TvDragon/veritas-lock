#ifndef MASTER_LOGIN_VIEW
#define MASTER_LOGIN_VIEW

#include "../data_handler/data_handler.h"
#include "data_context.h"

#define PASSWORD_LENGTH (25)

class MasterLoginView {
public:
	MasterLoginView(struct DataContext* ptrDataContext, DataHandler* ptrDatahandler);

	~MasterLoginView();

	void DisplayMasterLoginView();

private:
	struct DataContext* ptrDataContext;
	DataHandler* ptrDataHandler;
	char passwordBuff[PASSWORD_LENGTH] = "";
};

#endif