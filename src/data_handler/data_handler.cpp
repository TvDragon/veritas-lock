#include "data_handler.h"

DataHandler::DataHandler()
	:ptrDBHandler(new DatabaseHandler())
	, ptrEncryptionHandler(new EncryptionHandler())
{
	std::vector<std::string> users = GetUsers();
	if (users.size() != 0) {
		SetUser(users[0]);
	}
	std::vector<std::string> types = GetTypes();
	if (types.size() != 0) {
		GetLogins(types[0]);
		SetType(types[0]);
	}
}

DataHandler::~DataHandler() {
	delete ptrDBHandler;
	delete ptrEncryptionHandler;
}

void DataHandler::AddMasterLogin(std::string password) {
	password = CleanInput(password);
	password = ptrEncryptionHandler->EncryptMessage(password);
	ptrDBHandler->AddMasterLogin(password);
}

std::string DataHandler::GetMasterPassword() {
	std::string masterPassword = ptrDBHandler->GetMasterPassword();
	masterPassword = ptrEncryptionHandler->DecryptMessage(masterPassword);
	masterPassword = DecodeHTMLEntities(masterPassword);
	return masterPassword;
}

void DataHandler::SetType(std::string type) {
	std::string encryptedType = ptrEncryptionHandler->EncryptMessage(CleanInput(type));
	ptrDBHandler->SetType(encryptedType);
	resultsContext.currType = type;	// plain text Decoded type
}

std::vector<std::string> DataHandler::GetTypes() {
	std::vector<std::string> types = ptrDBHandler->GetTypes();

	resultsContext.types.clear();
	for (int i = 0; i < types.size(); i++) {
		types[i] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(types[i]));
		resultsContext.types.push_back(types[i]);
	}

	types.clear();
	return resultsContext.types;
}

std::string DataHandler::GetType() {
	return resultsContext.currType;
}

bool DataHandler::DeleteType(std::string type) {

	type = ptrEncryptionHandler->EncryptMessage(CleanInput(type));

	bool success = ptrDBHandler->DeleteType(type);

	if (success) {
		resultsContext.logins.clear();
	}

	return success;
}

void DataHandler::SetUser(std::string user) {
	std::string encryptedUser = ptrEncryptionHandler->EncryptMessage(CleanInput(user));
	ptrDBHandler->SetUser(encryptedUser);
	resultsContext.currUser = user;
}

std::vector<std::string> DataHandler::GetUsers() {
	std::vector<std::string> users = ptrDBHandler->GetUsers();

	resultsContext.users.clear();
	for (int i = 0; i < users.size(); i++) {
		users[i] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(users[i]));
		resultsContext.users.push_back(users[i]);
	}

	users.clear();
	return resultsContext.users;
}

std::string DataHandler::GetUser() {
	return resultsContext.currUser;
}

void DataHandler::GetLogins(std::string type) {
	std::string encryptedType = ptrEncryptionHandler->EncryptMessage(CleanInput(type));
	ptrDBHandler->GetLogins(encryptedType);
	resultsContext.currType = type;
	resultsContext.logins.clear();

	std::vector<std::map<std::string, std::string>> tempLogins = ptrDBHandler->GetLogins();
	for (int i = 0; i < tempLogins.size(); i++) {
		tempLogins[i]["account"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempLogins[i]["account"]));
		tempLogins[i]["type"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempLogins[i]["type"]));
		tempLogins[i]["username"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempLogins[i]["username"]));
		tempLogins[i]["email"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempLogins[i]["email"]));
		tempLogins[i]["password"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempLogins[i]["password"]));
		tempLogins[i]["notes"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempLogins[i]["notes"]));
		tempLogins[i]["website"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempLogins[i]["website"]));
		tempLogins[i]["user"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempLogins[i]["user"]));
		resultsContext.logins.push_back(tempLogins[i]);
	}

	tempLogins.clear();
}

std::vector<std::map<std::string, std::string>> DataHandler::GetLogins() {
	return resultsContext.logins;
}

std::vector<std::map<std::string, std::string>> DataHandler::GetAllLogins() {
	std::vector<std::map<std::string, std::string>> allLogins = ptrDBHandler->GetAllLogins();

	for (int i = 0; i < allLogins.size(); i++) {
		allLogins[i]["account"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(allLogins[i]["account"]));
		allLogins[i]["type"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(allLogins[i]["type"]));
		allLogins[i]["username"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(allLogins[i]["username"]));
		allLogins[i]["email"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(allLogins[i]["email"]));
		allLogins[i]["password"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(allLogins[i]["password"]));
		allLogins[i]["notes"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(allLogins[i]["notes"]));
		allLogins[i]["website"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(allLogins[i]["website"]));
		allLogins[i]["user"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(allLogins[i]["user"]));
	}

	return allLogins;
}

std::string DataHandler::AddLogin(std::string account, std::string type,
				std::string username, std::string email,
				std::string password, std::string notes,
				std::string website, std::string user) {

	account = ptrEncryptionHandler->EncryptMessage(CleanInput(account));
	type = ptrEncryptionHandler->EncryptMessage(CleanInput(type));
	username = ptrEncryptionHandler->EncryptMessage(CleanInput(username));
	email = ptrEncryptionHandler->EncryptMessage(CleanInput(email));
	password = ptrEncryptionHandler->EncryptMessage(CleanInput(password));
	notes = ptrEncryptionHandler->EncryptMessage(CleanInput(notes));
	website = ptrEncryptionHandler->EncryptMessage(CleanInput(website));
	user = ptrEncryptionHandler->EncryptMessage(CleanInput(user));
	
	std::string output = ptrDBHandler->AddLogin(account, type, username, email,
												password, notes, website, user);

	GetLogins(resultsContext.currType);
	
	return output;
}

void DataHandler::UpdateLogin(const int id, std::string account, std::string type,
				std::string username, std::string email,
				std::string password, std::string notes,
				std::string website, std::string user) {

	account = ptrEncryptionHandler->EncryptMessage(CleanInput(account));
	type = ptrEncryptionHandler->EncryptMessage(CleanInput(type));
	username = ptrEncryptionHandler->EncryptMessage(CleanInput(username));
	email = ptrEncryptionHandler->EncryptMessage(CleanInput(email));
	password = ptrEncryptionHandler->EncryptMessage(CleanInput(password));
	notes = ptrEncryptionHandler->EncryptMessage(CleanInput(notes));
	website = ptrEncryptionHandler->EncryptMessage(CleanInput(website));
	user = ptrEncryptionHandler->EncryptMessage(CleanInput(user));

	ptrDBHandler->UpdateLogin(id, account, type, username, email, password, notes, website, user);

	GetLogins(resultsContext.currType);
}

std::map<std::string, std::string> DataHandler::GetLogin(const int id) {
	std::map<std::string, std::string> tempMap = ptrDBHandler->GetLogin(id);

	tempMap["account"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempMap["account"]));
	tempMap["type"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempMap["type"]));
	tempMap["username"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempMap["username"]));
	tempMap["email"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempMap["email"]));
	tempMap["password"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempMap["password"]));
	tempMap["notes"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempMap["notes"]));
	tempMap["website"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempMap["website"]));

	return tempMap;
}

bool DataHandler::DeleteLogin(const int id, const bool isPartOfDeleteType) {
	bool success = ptrDBHandler->DeleteLogin(id, isPartOfDeleteType);

	if (success && !isPartOfDeleteType) {
		GetLogins(resultsContext.currType);
	}

	return success;
}

void DataHandler::GetUpdatedTrashLogins() {
	ptrDBHandler->GetUpdatedTrashLogins();

	std::vector<std::map<std::string, std::string>> tempTrashLogins = ptrDBHandler->GetTrashLogins();

	resultsContext.trashLogins.clear();
	for (int i = 0; i < tempTrashLogins.size(); i++) {
		tempTrashLogins[i]["account"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempTrashLogins[i]["account"]));
		tempTrashLogins[i]["type"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempTrashLogins[i]["type"]));
		tempTrashLogins[i]["username"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempTrashLogins[i]["username"]));
		tempTrashLogins[i]["email"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempTrashLogins[i]["email"]));
		tempTrashLogins[i]["password"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempTrashLogins[i]["password"]));
		tempTrashLogins[i]["notes"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempTrashLogins[i]["notes"]));
		tempTrashLogins[i]["website"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempTrashLogins[i]["website"]));
		tempTrashLogins[i]["user"] = DecodeHTMLEntities(ptrEncryptionHandler->DecryptMessage(tempTrashLogins[i]["user"]));
		resultsContext.trashLogins.push_back(tempTrashLogins[i]);
	}

	tempTrashLogins.clear();
}

std::vector<std::map<std::string, std::string>> DataHandler::GetTrashLogins() {
	return resultsContext.trashLogins;
}

bool DataHandler::DeleteFromTrash(const int id, const bool isPartOfDeleteAllTrash) {
	bool success = ptrDBHandler->DeleteFromTrash(id, isPartOfDeleteAllTrash);

	if (success && !isPartOfDeleteAllTrash) {
		GetUpdatedTrashLogins();
	}

	return success;
}

bool DataHandler::DeleteAllTrash() {
	bool success = ptrDBHandler->DeleteAllTrash();

	if (success) {
		resultsContext.trashLogins.clear();
	}

	return success;
}

void DataHandler::GetUpdatedTrashColumnNames() {
	ptrDBHandler->GetUpdatedTrashColumnNames();

	std::vector<std::string> tempColumnNames = ptrDBHandler->GetTrashColumnNames();

	resultsContext.trashColNames.clear();
	for (int i = 0; i < tempColumnNames.size(); i++) {
		resultsContext.trashColNames.push_back(tempColumnNames[i]);
	}

	tempColumnNames.clear();
}

std::vector<std::string> DataHandler::GetTrashColumnNames() {
	return resultsContext.trashColNames;
}