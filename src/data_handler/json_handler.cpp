#include "json_handler.h"

void ReadFile(const std::string filename,
							std::vector<std::vector<std::string>>& logins,
							std::vector<std::string>& failedReadLogins) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open the file: " << filename << "." << std::endl;
		exit(1);
	}

	// Read file contents into a string
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string fileContents = buffer.str();

	// Parse the JSON content
	struct json_object *jsonArrayContents;
	jsonArrayContents = json_tokener_parse(fileContents.c_str());
	if (jsonArrayContents == NULL) {
		std::cerr << "Failed to parse JSON." << std::endl;
		exit(1);
	}

	// Check if JSON contents is an array
	if (!json_object_is_type(jsonArrayContents, json_type_array)) {
		std::cerr << "The JSON file contents is not an array." << std::endl;
		exit(1);
	}

	// Iterate through elements in the array
	int arrLen = json_object_array_length(jsonArrayContents);
	for (int i = 0; i < arrLen; i++) {
		std::vector<std::string> login;
		struct json_object *jsonObj = json_object_array_get_idx(jsonArrayContents, i);

		// Extract fields from each JSON object in the array
        struct json_object *account, *type, *email, *username, *password, *notes, *website, *user;

		if (json_object_object_get_ex(jsonObj, "account", &account) &&
            json_object_object_get_ex(jsonObj, "type", &type) &&
            json_object_object_get_ex(jsonObj, "email", &email) &&
            json_object_object_get_ex(jsonObj, "username", &username) &&
            json_object_object_get_ex(jsonObj, "password", &password) &&
            json_object_object_get_ex(jsonObj, "notes", &notes) &&
            json_object_object_get_ex(jsonObj, "website", &website) &&
			json_object_object_get_ex(jsonObj, "user", &user)) {

            login.push_back(json_object_get_string(account));
            login.push_back(json_object_get_string(type));
            login.push_back(json_object_get_string(username));
            login.push_back(json_object_get_string(email));
            login.push_back(json_object_get_string(password));
            login.push_back(json_object_get_string(notes));
            login.push_back(json_object_get_string(website));
			login.push_back(json_object_get_string(user));
			logins.push_back(login);
        } else {
			std::stringstream ss;
			if (json_object_object_get_ex(jsonObj, "account", &account)) {
				ss << "Account: " << json_object_get_string(account);
			} else {
				ss << "Unidentified account has missing field";
			}
			failedReadLogins.push_back(ss.str());
		}
	}

	// Clean up the JSON object by freeing up memory
	json_object_put(jsonArrayContents);
}

bool WriteToFile(const std::string filename, const std::vector<std::map<std::string, std::string>> allLogins) {
	struct json_object *jsonArray = json_object_new_array();
	int numLogins = allLogins.size();
	
	for (int i = 0; i < numLogins; i++) {
		std::map<std::string, std::string> login = allLogins[i];
		struct json_object* jsonObj = json_object_new_object();
		json_object_object_add(jsonObj, "account", json_object_new_string(login["account"].c_str()));
		json_object_object_add(jsonObj, "type", json_object_new_string(login["type"].c_str()));
		json_object_object_add(jsonObj, "email", json_object_new_string(login["email"].c_str()));
		json_object_object_add(jsonObj, "username", json_object_new_string(login["username"].c_str()));
		json_object_object_add(jsonObj, "password", json_object_new_string(login["password"].c_str()));
		json_object_object_add(jsonObj, "notes", json_object_new_string(login["notes"].c_str()));
		json_object_object_add(jsonObj, "website", json_object_new_string(login["website"].c_str()));
		json_object_object_add(jsonObj, "user", json_object_new_string(login["user"].c_str()));

		json_object_array_add(jsonArray, jsonObj);
	}

	const char *jsonString = json_object_to_json_string_ext(jsonArray, JSON_C_TO_STRING_PRETTY);

	FILE *file = fopen(filename.c_str(), "w");
    if (file != NULL) {
        fprintf(file, "%s", jsonString);
        fclose(file);
        std::cout << "JSON Array written to file successfully." << std::endl;
    } else {
        perror("Failed to open file");
		return false;
    }

    // Step 5: Free JSON array
    json_object_put(jsonArray);
	
	return true;
}