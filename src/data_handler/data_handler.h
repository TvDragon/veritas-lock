#ifndef DATA_HANDLER
#define DATA_HANDLER

#include "database_handler.h"
#include "encryption_handler.h"

// Plain Text Decoded results
struct ResultsContext {
	std::vector<std::string> types;
	std::vector<std::string> users;
	std::string currType;
	std::string currUser;
	std::vector<std::map<std::string, std::string>> logins;
	std::vector<std::map<std::string, std::string>> trashLogins;
	std::vector<std::string> trashColNames;
};


class DataHandler {
public:
	DataHandler();

	~DataHandler();

	void AddMasterLogin(std::string password);

	std::string GetMasterPassword();
	
	void SetType(std::string type);
	
	std::vector<std::string> GetTypes();
	
	std::string GetType();

	bool DeleteType(std::string type);

	void SetUser(std::string user);

	std::vector<std::string> GetUsers();

	std::string GetUser();

	void GetLogins(std::string type);

	std::vector<std::map<std::string, std::string>> GetLogins();

	std::vector<std::map<std::string, std::string>> GetAllLogins();
	
	std::string AddLogin(std::string account, std::string type,
					std::string username, std::string email,
					std::string password, std::string notes,
					std::string website, std::string user);

	void UpdateLogin(const int id, std::string account, std::string type,
					std::string username, std::string email,
					std::string password, std::string notes,
					std::string website, std::string user);

	std::map<std::string, std::string> GetLogin(const int id);

	bool DeleteLogin(const int id, const bool isPartOfDeleteType = false);

	void GetUpdatedTrashLogins();

	std::vector<std::map<std::string, std::string>> GetTrashLogins();

	bool DeleteFromTrash(const int id, const bool isPartOfDeleteAllTrash = false);

	bool DeleteAllTrash();

	void GetUpdatedTrashColumnNames();

	std::vector<std::string> GetTrashColumnNames();

private:
	DatabaseHandler* ptrDBHandler;
	EncryptionHandler* ptrEncryptionHandler;
	struct ResultsContext resultsContext;
};

#endif