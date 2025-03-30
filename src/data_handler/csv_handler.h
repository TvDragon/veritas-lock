#ifndef CSV_HANDLER
#define CSV_HANDLER

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>


void ReadCSVFile(const std::string filename, std::vector<std::vector<std::string>> &logins,
				std::vector<std::string> &failedReadLogins);

bool WriteToCSVFile(const std::string filename, const std::vector<std::map<std::string, std::string>> allLogins);

#endif