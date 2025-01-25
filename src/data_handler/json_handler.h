#ifndef JSON_HANDLER
#define JSON_HANDLER

#include <json-c/json.h>

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

void ReadFile(const std::string filename, std::vector<std::vector<std::string>> &logins,
				std::vector<std::string> &failedReadLogins);

bool WriteToFile(const std::string filename, const std::vector<std::map<std::string, std::string>> allLogins);

#endif
