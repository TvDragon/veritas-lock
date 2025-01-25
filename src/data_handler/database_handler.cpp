#include "database_handler.h"

DatabaseLog::DatabaseLog() {}

DatabaseLog::~DatabaseLog() {}

void DatabaseLog::LogMessage(const std::string message) {
#ifdef DEBUG
	logFile.open("messages.log", std::ios_base::app);	// Contains SQL queries and messages
	logFile << message;
	logFile.close();
#endif
}

DatabaseHandler::DatabaseHandler()
	:dbLog(new DatabaseLog())
	, updatedDB(true)
	, doesDBExist(false)
{
	int returnCode = 0;
	std::stringstream ss;

	std::ifstream dbFile("logins.db");

	if (dbFile.is_open()) {
		doesDBExist = true;
	}

	returnCode = sqlite3_open("logins.db", &db);

	if (returnCode) {
		ss << "\tError opening DB " << sqlite3_errmsg(db) << ".\n\n";
		dbLog->LogMessage(ss.str());
		exit(1);
	} else {
		ss << "Opened Database Successfully.\n\n";
		dbLog->LogMessage(ss.str());
		if (doesDBExist) {
			std::vector<std::string> users = GetUsers();
			if (users.size() != 0) {
				SetUser(users[0]);
			}
			std::vector<std::string> types =  GetTypes();
			if (types.size() != 0) {
				GetLogins(types[0]);
				SetType(types[0]);
			}
		}
	}

	if (!doesDBExist) {
		CreateTable();
	}
}

DatabaseHandler::~DatabaseHandler() {
	int returnCode = sqlite3_close(db);
	std::stringstream ss;
	
	if (returnCode) {
		ss << "\tError closing DB: " << sqlite3_errmsg(db) << ".\n\n";
		dbLog->LogMessage(ss.str());
	} else {
		ss << "Closed Database Successfully.\n\n";
		dbLog->LogMessage(ss.str());
	}
	delete dbLog;
}

void DatabaseHandler::ExecuteSQL(const std::string sqlQuery) {
	char* messageError = NULL;
	std::stringstream ss;
	int returnCode = sqlite3_exec(db, sqlQuery.c_str(), NULL, 0, &messageError);

	if (returnCode != SQLITE_OK) {
		ss << "Error excuting SQL: " << messageError << "\n\n";
		dbLog->LogMessage(ss.str());
		sqlite3_free(messageError);
		throw std::runtime_error("SQL Execution Error");
	}
}

void DatabaseHandler::CreateTable() {
	std::string sqlQuery = "CREATE TABLE Logins ("
							"id INTEGER PRIMARY KEY AUTOINCREMENT, "
							"account	TINYTEXT	NOT NULL, "
							"type		VARCHAR(30)	NOT NULL, "
							"username	TINYTEXT	NOT NULL, "
							"email		TINYTEXT	NOT NULL, "
							"password	TINYTEXT	NOT NULL, "
							"notes		TEXT		NOT NULL, "
							"website	TEXT		NOT NULL, "
							"user		TEXT		NOT NULL);";
	std::stringstream ss;

	try {
		ExecuteSQL("BEGIN TRANSACTION;");
		dbLog->LogMessage(sqlQuery + "\n");
		ExecuteSQL(sqlQuery);
		sqlQuery = "CREATE TABLE Trash ("
					"id INTEGER PRIMARY KEY	NOT NULL, "
					"account	TINYTEXT	NOT NULL, "
					"type		VARCHAR(30)	NOT NULL, "
					"username	TINYTEXT	NOT NULL, "
					"email		TINYTEXT	NOT NULL, "
					"password	TINYTEXT	NOT NULL, "
					"notes		TEXT		NOT NULL, "
					"website	TEXT		NOT NULL, "
					"user		TEXT		NOT NULL);";
		dbLog->LogMessage(sqlQuery + "\n");
		ExecuteSQL(sqlQuery);
		sqlQuery = "CREATE TABLE Master_Login ("
					"id	INTEGER PRIMARY KEY	NOT NULL,"
					"password	TINYTEXT	NOT NULL);";
		dbLog->LogMessage(sqlQuery + "\n");
		ExecuteSQL(sqlQuery);
		ExecuteSQL("COMMIT;");
		ss << "Created Table Logins.\nCreated Table Trash.\n\n";
	} catch (const std::exception& e) {
		ExecuteSQL("ROLLBACK;");
		ss << "Transaction rolled back due to error: " << e.what() << "\n\n";
	}

	dbLog->LogMessage(ss.str());
}

void DatabaseHandler::AddMasterLogin(std::string password) {
	std::stringstream ss;
	ss << "INSERT INTO Master_Login (password) VALUES"
		<< "('" << password << "');";
	std::string sqlQuery = ss.str();

	ss.str("");

	try {
		ExecuteSQL("BEGIN TRANSACTION;");
		dbLog->LogMessage(sqlQuery + "\n");
		ExecuteSQL(sqlQuery);
		ExecuteSQL("COMMIT;");
		ss << "Added Master Login Successfully.\n\n";
		dbLog->LogMessage(ss.str());
	} catch (const std::exception& e) {
		ExecuteSQL("ROLLBACK;");
		ss << "Transaction rolled back due to error: " << e.what() << "\n\n";
		dbLog->LogMessage(ss.str());
		ss.str("");
		ss << "Failed to import for Master Login.\n\n";
		dbLog->LogMessage(ss.str());
	}
}

std::string DatabaseHandler::GetMasterPassword() {
	std::stringstream ss;
	char* messageError = NULL;
	int returnCode = 0;
	std::vector<std::map<std::string, std::string>> tempLogins;

	try {
		std::string sqlQuery = "SELECT * FROM Master_Login;";
		returnCode = sqlite3_exec(db, sqlQuery.c_str(), callBackGetLogins,
									&tempLogins, &messageError);

		dbLog->LogMessage(sqlQuery + "\n");
		ss.str("");
		if (returnCode != SQLITE_OK) {
			ss << "Error excuting SQL: " << messageError << "\n\n";
			dbLog->LogMessage(ss.str());
			sqlite3_free(messageError);
			throw std::runtime_error("SQL Execution Error");
		} else {
			ss << "Retrieved Master Login Successfully.\n\n";
			dbLog->LogMessage(ss.str());

			if (tempLogins.size() != 0) {
				return tempLogins[0]["password"];
			}
		}
	} catch (const std::exception& e) {
		ss.str("");
		ss << "Failed to retrieve Master Login due to error: " << e.what() << "\n\n";
		dbLog->LogMessage(ss.str());
	}
	
	return "";
}

std::string DatabaseHandler::AddLogin(std::string account, std::string type,
					std::string username, std::string email,
					std::string password, std::string notes,
					std::string website, std::string user) {
	std::stringstream ss;
	ss << "INSERT INTO Logins (account, type, username, email, password, " <<
		"notes, website, user) VALUES ('" << account << "', '" << type << "', '"
		<< username << "', '" << email << "', '" << password << "', '" << notes
		<< "', '" << website << "', '" << user << "');";
	std::string sqlQuery = ss.str();

	ss.str("");
	
	try {
		ExecuteSQL("BEGIN TRANSACTION;");
		dbLog->LogMessage(sqlQuery + "\n");
		ExecuteSQL(sqlQuery);
		ExecuteSQL("COMMIT;");
		ss << "Added Login Successfully.\n\n";
		dbLog->LogMessage(ss.str());
		GetLogins(dbResultsContext.currType);
	} catch (const std::exception& e) {
		ExecuteSQL("ROLLBACK;");
		ss << "Transaction rolled back due to error: " << e.what() << "\n\n";
		dbLog->LogMessage(ss.str());
		ss.str("");
		ss << "Failed to import for login: (account, type, username, email)\n";
		ss << account << ", " << type << ", " << username << ", " << email << "\n\n";
		return ss.str();
	}
	return "";
}
void DatabaseHandler::AddLoginToTrash(const int id, std::map<std::string, std::string> login) {
	std::stringstream ss;
	
	std::string account = login["account"];
	std::string type = login["type"];
	std::string username = login["username"];
	std::string email = login["email"];
	std::string password = login["password"];
	std::string notes = login["notes"];
	std::string website = login["website"];
	std::string user = login["user"];

	if (!login.empty()) {
		ss << "Insert INTO Trash (id, account, type, username, email, password, " <<
			"notes, website, user) VALUES (" << login["id"] << ", '" << account <<
			"', '" << type << "', '" << username << "', '" << email << "', '" <<
			password << "', '" << notes << "', '" << website << "', '" << user << "');";
		std::string sqlQuery = ss.str();

		dbLog->LogMessage(sqlQuery + "\n");
		ExecuteSQL(sqlQuery);
	} else {
		ss << "Failed to Add Login id: " << id << " to Trash as does not exist.\n\n";
		dbLog->LogMessage(ss.str());
	}
}

void DatabaseHandler::SetType(const std::string type) {
	dbResultsContext.currType = type;
}

/* 
	data - Data provided in the 4th argument of sqlite3_exec()
	argc - The number of columns in row
	argv - An array of strings representing fields in the row
	azColNames - An array of strings representing column names
*/
static int callBackGetTypes(void* data, int argc, char** argv, char** azColNames) {
	std::vector<std::string>* tempTypes = (std::vector<std::string>*) data;
	
	for (int i = 0; i < argc; i++) {
		tempTypes->push_back(argv[i]);
	}

	return 0;
}

std::vector<std::string> DatabaseHandler::GetTypes() {
	std::stringstream ss;
	char* messageError = NULL;
	int returnCode = 0;
	std::vector<std::string> tempTypes;

	try {
		std::string sqlQuery = "SELECT DISTINCT type from Logins WHERE user='" + dbResultsContext.currUser + "';";
		// If returnCode is not successful then restore dbResultsContext.types back. May need to store dup vector.
		returnCode = sqlite3_exec(db, sqlQuery.c_str(), callBackGetTypes,
									&tempTypes, &messageError);

		dbLog->LogMessage(sqlQuery + "\n");
		if (returnCode != SQLITE_OK) {
			ss << "Error excuting SQL: " << messageError << "\n\n";
			dbLog->LogMessage(ss.str());
			sqlite3_free(messageError);
			throw std::runtime_error("SQL Execution Error");
		} else {
			ss << "Retrieved Types Successfully.\n\n";
			dbLog->LogMessage(ss.str());
			dbResultsContext.types.clear();
			for (int i = 0; i < tempTypes.size(); i++) {
				dbResultsContext.types.push_back(tempTypes[i]);
			}
		}
	} catch (const std::exception& e) {
		ss.str("");
		ss << "Failed to retrieve Types due to error: " << e.what() << "\n\n";
		dbLog->LogMessage(ss.str());
	}

	tempTypes.clear();
	return dbResultsContext.types;
}

std::string DatabaseHandler::GetType() {
	return dbResultsContext.currType;
}

bool DatabaseHandler::DeleteType(const std::string type) {
	std::vector<std::map<std::string, std::string>> logins = dbResultsContext.logins;
	int numLogins = logins.size();
	std::stringstream ss;

	try {
		ExecuteSQL("BEGIN TRANSACTION;");
		for (int i = 0; i < numLogins; i++) {
			std::map<std::string, std::string> login = logins[i];
			DeleteLogin(std::stoi(login["id"]), true);
		}
		ExecuteSQL("COMMIT;");
		dbResultsContext.logins.clear();
		if (numLogins != 0) {
			ss << "Deleted Type: " << type << " Successfully.\n\n";
		} else {
			ss << "Failed to Delete Type: " << type << " as does not exist.\n\n";
		}
		dbLog->LogMessage(ss.str());
		return true;
	} catch (const std::exception& e) {
		ExecuteSQL("ROLLBACK;");
		ss << "Failed to Delete Type: " << type << "\n\n";
		dbLog->LogMessage(ss.str());
	}
	return false;
}

void DatabaseHandler::SetUser(const std::string user) {
	dbResultsContext.currUser = user;
}

static int callBackGetUsers(void* data, int argc, char** argv, char** azColNames) {
	std::vector<std::string>* tempUsers = (std::vector<std::string>*) data;

	for (int i = 0; i < argc; i++) {
		tempUsers->push_back(argv[i]);
	}

	return 0;
}

std::vector<std::string> DatabaseHandler::GetUsers() {
	std::stringstream ss;
	char* messageError = NULL;
	int returnCode = 0;
	std::vector<std::string> tempUsers;

	try {
		std::string sqlQuery = "SELECT DISTINCT user from Logins;";
		returnCode = sqlite3_exec(db, sqlQuery.c_str(), callBackGetUsers,
									&tempUsers, &messageError);
		
		dbLog->LogMessage(sqlQuery + "\n");
		if (returnCode != SQLITE_OK) {
			ss << "Error excuting SQL: " << messageError << "\n\n";
			dbLog->LogMessage(ss.str());
			sqlite3_free(messageError);
			throw std::runtime_error("SQL Execution Error");
		} else {
			ss << "Retrieved Users Successfully.\n\n";
			dbLog->LogMessage(ss.str());
			dbResultsContext.users.clear();
			for (int i = 0; i < tempUsers.size(); i++) {
				dbResultsContext.users.push_back(tempUsers[i]);
			}
		}
	} catch (const std::exception& e) {
		ss.str("");
		ss << "Failed to retrieve Users due to error: " << e.what() << "\n\n";
		dbLog->LogMessage(ss.str());
	}

	tempUsers.clear();
	return dbResultsContext.users;
}

std::string DatabaseHandler::GetUser() {
	return dbResultsContext.currUser;
}

static int callBackGetLogins(void* data, int argc, char** argv, char** azColNames) {
	std::vector<std::map<std::string, std::string>>* logins = (std::vector<std::map<std::string, std::string>>*) data;
	
	std::map<std::string, std::string> login;
	for (int i = 0; i < argc; i++) {
		login[azColNames[i]] = argv[i];
	}

	logins->push_back(login);
	return 0;
}

void DatabaseHandler::GetLogins(const std::string type) {
	std::stringstream ss;
	char* messageError = NULL;
	int returnCode = 0;
	std::vector<std::map<std::string, std::string>> tempLogins;

	try {
		ss << "SELECT * FROM Logins WHERE type='" << type << "' AND user='" << dbResultsContext.currUser << "';";
	
		std::string sqlQuery = ss.str();
		// If returnCode is not successful then restore dbResultsContext.logins back. May need to store dup vector.
		returnCode = sqlite3_exec(db, sqlQuery.c_str(), callBackGetLogins,
									&tempLogins, &messageError);

		dbLog->LogMessage(sqlQuery + "\n");
		ss.str("");
		if (returnCode != SQLITE_OK) {
			ss << "Error excuting SQL: " << messageError << "\n\n";
			dbLog->LogMessage(ss.str());
			sqlite3_free(messageError);
			throw std::runtime_error("SQL Execution Error");
		} else {
			ss << "Retrieved Logins Successfully.\n\n";
			dbLog->LogMessage(ss.str());
			dbResultsContext.currType = type;
			dbResultsContext.logins.clear();
			for (int i = 0; i < tempLogins.size(); i++) {
				dbResultsContext.logins.push_back(tempLogins[i]);
			}
		}
	} catch (const std::exception& e) {
		ss.str("");
		ss << "Failed to retrieve Logins due to error: " << e.what() << "\n\n";
		dbLog->LogMessage(ss.str());
	}

	tempLogins.clear();
}

std::vector<std::map<std::string, std::string>> DatabaseHandler::GetLogins() {
	return dbResultsContext.logins;
}

std::vector<std::map<std::string, std::string>> DatabaseHandler::GetAllLogins() {
	std::stringstream ss;
	char* messageError = NULL;
	int returnCode = 0;
	std::vector<std::map<std::string, std::string>> allLogins;

	try {
		std::string sqlQuery = "SELECT * FROM Logins;";
		returnCode = sqlite3_exec(db, sqlQuery.c_str(), callBackGetLogins,
									&allLogins, &messageError);

		dbLog->LogMessage(sqlQuery + "\n");
		ss.str("");
		if (returnCode != SQLITE_OK) {
			ss << "Error executing SQL:" << messageError << "\n\n";
			dbLog->LogMessage(ss.str());
			sqlite3_free(messageError);
			throw std::runtime_error("SQL Execution Error");
		} else {
			ss << "Retrieved Logins Successfully.\n\n";
			dbLog->LogMessage(ss.str());
		}
	} catch (const std::exception& e) {
		ss.str("");
		ss << "Failed to retrive all Logins due to error: " << e.what() << "\n\n";
		dbLog->LogMessage(ss.str());
	}
	return allLogins;
}

void DatabaseHandler::UpdateLogin(const int id, std::string account,
					std::string type, std::string username,
					std::string email, std::string password,
					std::string notes, std::string website,
					std::string user) {

	std::stringstream ss;
	int numAffectedRows = 0;
	ss << "UPDATE Logins SET account='" << account << "', ";
	ss << "type='" << type << "', username='" << username << "', ";
	ss << "email='" << email << "', password='" << password << "', ";
	ss << "notes='" << notes << "', website='" << website << "', ";
	ss << "user='" << user << "' " << "WHERE id=" << id << ";";

	std::string sqlQuery = ss.str();
	ss.str("");

	try {
		ExecuteSQL("BEGIN TRANSACTION;");
		dbLog->LogMessage(sqlQuery + "\n");
		ExecuteSQL(sqlQuery);
		ExecuteSQL("COMMIT;");
		numAffectedRows = sqlite3_changes(db);
		if (numAffectedRows > 0) {
			ss << "Updated Login Successfully.\n\n";
			dbLog->LogMessage(ss.str());
			GetLogins(dbResultsContext.currType);
		} else {
			ss << "Failed To Update Login id: " << id << " as does not exist.\n\n";
			dbLog->LogMessage(ss.str());
		}
	} catch (const std::exception& e) {
		ExecuteSQL("ROLLBACK;");
		ss << "Transaction rolled back due to error: " << e.what() << "\n\n";
		dbLog->LogMessage(ss.str());
	}
}

std::map<std::string, std::string> DatabaseHandler::GetLogin(const int id) {
	std::stringstream ss;
	std::map<std::string, std::string> tempMap;
	int numLogins = dbResultsContext.logins.size();
	if (numLogins != 0) {
		for (int i = 0; i < numLogins; i++) {
			if (stoi(dbResultsContext.logins[i]["id"]) == id) {
				ss << "Retrived Login for type: " << dbResultsContext.currType << ", id: " << id << ".\n\n";
				tempMap["id"] = dbResultsContext.logins[i]["id"];
				tempMap["account"] = dbResultsContext.logins[i]["account"];
				tempMap["type"] = dbResultsContext.logins[i]["type"];
				tempMap["username"] = dbResultsContext.logins[i]["username"];
				tempMap["email"] = dbResultsContext.logins[i]["email"];
				tempMap["password"] = dbResultsContext.logins[i]["password"];
				tempMap["notes"] = dbResultsContext.logins[i]["notes"];
				tempMap["website"] = dbResultsContext.logins[i]["website"];
				tempMap["user"] = dbResultsContext.logins[i]["user"];
				break;
			}
		}
	}
	if (tempMap.empty()) {
		ss << "Failed to Get Login for type: " << dbResultsContext.currType << ", id: " << id << ".\n\n";
	}
	dbLog->LogMessage(ss.str());

	return tempMap;
}

bool DatabaseHandler::DeleteLogin(const int id, const bool isPartOfDeleteType) {
	std::stringstream ss;
	char* messageError = NULL;
	int returnCode = 0;
	int numAffectedRows = 0;
	std::map<std::string, std::string> login = GetLogin(id);
	ss << "DELETE FROM Logins WHERE id=" << id << ";";

	std::string sqlQuery = ss.str();
	ss.str("");

	try {
		if (!isPartOfDeleteType) {
			ExecuteSQL("BEGIN TRANSACTION;");
		}
		AddLoginToTrash(id, login);
		dbLog->LogMessage(sqlQuery + "\n");
		ExecuteSQL(sqlQuery);
		if (!isPartOfDeleteType) {
			ExecuteSQL("COMMIT;");
		}
		numAffectedRows = sqlite3_changes(db);
		if (numAffectedRows > 0) {
			ss << "Added Login into Trash Successfully for id: " << id;
			ss << ".\nDeleted Login Successfully for id: " << id << ".\n\n";
		} else {
			ss << "Failed to Delete Login id: " << id << " as does not exist.\n\n";
		}
		dbLog->LogMessage(ss.str());
		if (numAffectedRows > 0 && !isPartOfDeleteType) {	// Message for DeleteLogin needs to be written first
			GetLogins(dbResultsContext.currType);
		}
	} catch (const std::exception& e) {
		if (!isPartOfDeleteType) {
			ExecuteSQL("ROLLBACK;");
		}
		ss << "Transaction rolled back due to error: " << e.what() << "\n\n";
		dbLog->LogMessage(ss.str());
		if (isPartOfDeleteType) {
			throw std::runtime_error("SQL Execution Error");
		}
		return false;
	}
	return true;
}

void DatabaseHandler::GetUpdatedTrashLogins() {
	std::stringstream ss;
	char* messageError = NULL;
	int returnCode = 0;
	std::vector<std::map<std::string, std::string>> tempTrashLogins;

	try {
		ss << "SELECT * FROM Trash;";

		std::string sqlQuery = ss.str();
		returnCode = sqlite3_exec(db, sqlQuery.c_str(), callBackGetLogins,
									&tempTrashLogins, &messageError);

		dbLog->LogMessage(sqlQuery + "\n");
		ss.str("");
		if (returnCode != SQLITE_OK) {
			ss << "Error executing SQL: " << messageError << "\n\n";
			dbLog->LogMessage(ss.str());
			sqlite3_free(messageError);
			throw std::runtime_error("SQL Execution Error");
		} else {
			ss << "Retrieved Logins From Trash Successfully.\n\n";
			dbLog->LogMessage(ss.str());
			dbResultsContext.trashLogins.clear();
			for (int i = 0; i < tempTrashLogins.size(); i++) {
				dbResultsContext.trashLogins.push_back(tempTrashLogins[i]);
			}
		}
	} catch (const std::exception& e) {
		ss.str("");
		ss << "Failed to retrieve Trash Logins due to error: " << e.what() << "\n\n";
		dbLog->LogMessage(ss.str());
	}

	tempTrashLogins.clear();
}

std::vector<std::map<std::string, std::string>> DatabaseHandler::GetTrashLogins() {
	return dbResultsContext.trashLogins;
}

bool DatabaseHandler::DeleteFromTrash(const int id, const bool isPartOfDeleteAllTrash) {
	std::stringstream ss;
	char* messageError = NULL;
	int returnCode = 0;
	int numAffectedRows = 0;
	ss << "DELETE FROM Trash WHERE id=" << id << ";";

	std::string sqlQuery = ss.str();
	ss.str("");

	try {
		if (!isPartOfDeleteAllTrash) {
			ExecuteSQL("BEGIN TRANSACTION;");
		}
		dbLog->LogMessage(sqlQuery + "\n");
		ExecuteSQL(sqlQuery);
		if (!isPartOfDeleteAllTrash) {
			ExecuteSQL("COMMIT;");
		}
		numAffectedRows = sqlite3_changes(db);
		if (numAffectedRows > 0) {
			ss << "Deleted Login from Trash Successfully for id: " << id << ".\n\n";
		} else {
			ss << "Failed to Delete Trash Login id: " << id << " as does not exist.\n\n";
		}
		dbLog->LogMessage(ss.str());
		if (numAffectedRows > 0 && !isPartOfDeleteAllTrash) {
			GetUpdatedTrashLogins();
		}
	} catch (const std::exception& e) {
		if (!isPartOfDeleteAllTrash) {
			ExecuteSQL("ROLLBACK;");
		}
		ss << "Transaction rolled back due to error: " << e.what() << "\n\n";
		dbLog->LogMessage(ss.str());
		if (isPartOfDeleteAllTrash) {
			throw std::runtime_error("SQL Execution Error");
		}
		return false;
	}

	return true;
}

bool DatabaseHandler::DeleteAllTrash() {
	std::vector<std::map<std::string, std::string>> trashLogins = dbResultsContext.trashLogins;
	int numLogins = trashLogins.size();
	std::stringstream ss;

	try {
		ExecuteSQL("BEGIN TRANSACTION;");
		for (int i = 0; i < numLogins; i++) {
			std::map<std::string, std::string> login = trashLogins[i];
			DeleteFromTrash(std::stoi(login["id"]), true);
		}
		ExecuteSQL("COMMIT;");
		dbResultsContext.trashLogins.clear();
		ss << "Deleted Login from Trash Successfully.\n\n";
		dbLog->LogMessage(ss.str());
		return true;
	} catch (const std::exception& e) {
		ExecuteSQL("ROLLBACK;");
		ss << "Failed to Delete All Trash.\n\n";
		dbLog->LogMessage(ss.str());
	}

	return false;
}

static int callBackGetTrashColumnNames(void* data, int argc, char** argv, char** azColName) {
	std::vector<std::string>* colNames = (std::vector<std::string>*) data;
	for (int i = 0; i < argc; i++) {
		if (strcmp(azColName[i], "name") == 0 && argv[i]) {
			colNames->push_back(std::string(argv[i]));
		}
	}

	return 0;
}

void DatabaseHandler::GetUpdatedTrashColumnNames() {
	std::stringstream ss;
	char* messageError = NULL;
	int returnCode = 0;
	std::vector<std::string> tempColumnNames;

	try {
		ss << "PRAGMA table_info('Trash')";
		std::string sqlQuery = ss.str();

		returnCode = sqlite3_exec(db, sqlQuery.c_str(), callBackGetTrashColumnNames,
									&tempColumnNames, &messageError);

		dbLog->LogMessage(sqlQuery + "\n");
		ss.str("");
		if (returnCode != SQLITE_OK) {
			ss << "Error executing SQL: " << messageError << "\n\n";
			dbLog->LogMessage(ss.str());
			sqlite3_free(messageError);
			throw std::runtime_error("SQL Execution Error");
		} else {
			ss << "Retrieved Trash Column Names Successfully.\n\n";
			dbLog->LogMessage(ss.str());
			dbResultsContext.trashColNames.clear();
			for (int i = 0; i < tempColumnNames.size(); i++) {
				dbResultsContext.trashColNames.push_back(tempColumnNames[i]);
			}
		}
	} catch (const std::exception& e) {
		ss.str("");
		ss << "Error occured from : " << e.what() << "\n\n";
		dbLog->LogMessage(ss.str());
	}

	tempColumnNames.clear();
}

std::vector<std::string> DatabaseHandler::GetTrashColumnNames() {
	return dbResultsContext.trashColNames;
}