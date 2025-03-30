#include "csv_handler.h"

void ReadCSVFile(const std::string filename, std::vector<std::vector<std::string>> &logins,
				std::vector<std::string> &failedReadLogins) {

	std::ifstream ifs(filename);
	if (!ifs.is_open()) {
		std::cerr << "Failed to open the file: " << filename << "." << std::endl;
		exit(1);
	}

	// Read the file character by character
	char ch;
	std::stringstream ss;
	std::string line, t;
	int colIdx = 1;
	int numCols = 0;
	std::vector<std::string> login;

	// Extract first line to get column names
	std::getline(ifs, line);
	std::stringstream ssl(line);
	while (std::getline(ssl, t, ',')) {
		numCols++;
	}

	// Read character line by line to get row for login
	while (ifs.get(ch)) {
		if (ch == ',') {
			if (colIdx == 6) {	// Notes which have spaces in them will be enclosed by quotation marks
				std::string notes = ss.str();
				if (notes.length() > 2) {
					if (notes[0] == '"' && notes[notes.length() - 1] == '"') {
						notes = notes.substr(1, notes.length() - 2);
					}
				}
				login.push_back(notes);
			} else {
				login.push_back(ss.str());
			}
			ss.str("");
			colIdx++;
			if (colIdx == numCols) {	// Retrieve last column info
				while (ifs.get(ch)) {
					if (ch == '\n') {
						break;
					}
					ss << ch;
				}
				login.push_back(ss.str());
				logins.push_back(login);
				login.clear();	// New login account details
				ss.str("");
				colIdx = 1;
			}
		} else {
			ss << ch;
		}
	}

	ifs.close();
	login.clear();
}

bool WriteToCSVFile(const std::string filename, const std::vector<std::map<std::string, std::string>> allLogins) {
	int numLogins = allLogins.size();
	std::ofstream ofs(filename);

	if (!ofs.is_open()) {
		std::cerr << "Failed to open the file: " << filename << "." << std::endl;
		exit(1);
	}

	ofs << "account,type,username,email,password,notes,website,user\n";

	for (int i = 0; i < numLogins; i++) {
		std::map<std::string, std::string> login = allLogins[i];
		ofs << login["account"] << ",";
		ofs << login["type"] << ",";
		ofs << login["username"] << ",";
		ofs << login["email"] << ",";
		ofs << login["password"] << ",";
		ofs << login["notes"] << ",";
		ofs << login["website"] << ",";
		ofs << login["user"] << "\n";
	}
	
	ofs.close();

	return true;
}