#ifndef DATABASE_HANDLER
#define DATABASE_HANDLER

#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <string.h>

#include "util.h"

static int callBackGetLogins(void* data, int argc, char** argv, char** azColNames);

class DatabaseLog {
public:
	DatabaseLog();

	~DatabaseLog();

	void LogMessage(const std::string message);

private:
	std::ofstream logFile;
};

// Encrypted results from DB
struct DBResultsContext {
	std::vector<std::string> types;
	std::vector<std::string> users;
	std::string currType;
	std::string currUser;
	std::vector<std::map<std::string, std::string>> logins;
	std::vector<std::map<std::string, std::string>> trashLogins;
	std::vector<std::string> trashColNames;
};

class DatabaseHandler {
public:
	DatabaseHandler();

	~DatabaseHandler();

	void ExecuteSQL(const std::string sqlQuery);

	void CreateTable();

	void AddMasterLogin(std::string password);

	std::string GetMasterPassword();

	void SetType(const std::string type);
	
	std::vector<std::string> GetTypes();

	std::string GetType();

	bool DeleteType(const std::string type);

	void SetUser(const std::string user);

	std::vector<std::string> GetUsers();

	std::string GetUser();

	void GetLogins(const std::string type);

	std::vector<std::map<std::string, std::string>> GetLogins();

	std::vector<std::map<std::string, std::string>> GetAllLogins();
	
	std::string AddLogin(std::string account, std::string type,
					std::string username, std::string email,
					std::string password, std::string notes,
					std::string website, std::string user);

	void AddLoginToTrash(const int id, std::map<std::string, std::string> login);

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
	sqlite3 *db;
	DatabaseLog *dbLog;
	struct DBResultsContext dbResultsContext;
	bool updatedDB;
	bool doesDBExist;
};

#endif