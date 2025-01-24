#include "util.h"

std::string cleanInput(std::string inputStr) {
	std::map<char, std::string> htmlEntities = {
        {'&', "&amp;"},
        {';', "&#59;"},
        {'<', "&lt;"},
        {'>', "&gt;"},
        {'"', "&quot;"},
        {'\'', "&#39;"} // single quote
    };

	std::stringstream ss;

	for (int i = 0; i < inputStr.size(); i++) {
		if (htmlEntities.find(inputStr[i]) == htmlEntities.end()) {
			ss << inputStr[i];
		} else {
			ss << htmlEntities[inputStr[i]];
		}
	}

	inputStr = ss.str();
	return inputStr;
}

std::string decodeHTMLEntities(std::string inputStr) {
	std::map<std::string, char> htmlEntities = {
        {"&amp;", '&'},
        {"&#59;", ';'},
        {"&lt;", '<'},
        {"&gt;", '>'},
        {"&quot;", '"'},
        {"&#39;", '\''}  // single quote
    };

	std::stringstream ss;

	for (int i = 0; i < inputStr.size(); i++) {
		// Only check for entities if we see an '&' indicating the start of an entity
        if (inputStr[i] == '&') {
            size_t semicolonPos = inputStr.find(';', i);
            if (semicolonPos != std::string::npos) {	// largest position possible meaning not at end
                // Extract the full potential entity
                std::string entity = inputStr.substr(i, semicolonPos - i + 1);
                
                // Check if this entity exists in our map
                if (htmlEntities.count(entity)) {
                    ss << htmlEntities[entity];
                    i = semicolonPos; // Skip past the entity
                    continue;
                }
            }
        }
        // If not an entity, or if no valid entity found, add the character as-is
        ss << inputStr[i];
	}

	return ss.str();
}