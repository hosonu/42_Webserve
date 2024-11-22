#include <cstring>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>

int main() {
	std::string filePath = "../config/test.conf";

	std::ifstream file(filePath.c_str());
	std::stringstream streamConf;
	streamConf << file.rdbuf();
	
	std::string	token;
	std::vector<std::string> tokens; 
	while (streamConf >> token) {
		std::cout << token << std::endl;
		if (token.back() == ';') {
			std::string word = token.substr(0, token.size() - 1);
			tokens.push_back(word);
			tokens.push_back(";");
		} else {
			tokens.push_back(token);
		}
	}

	for (const auto& token : tokens) {
		std::cout << "Token: " << token << std::endl;
	}

	return 0;
}
