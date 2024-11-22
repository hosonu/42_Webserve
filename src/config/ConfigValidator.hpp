#ifndef CONFIGVALIDATOR
#define CONFIGVALIDATOR

#include <vector>
#include <string>
#include <set>
#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>

class ConfigValidator {
	private:
		std::set<std::string> valid_server_directives;
		std::set<std::string> valid_location_directives;

		void	initializeValidServerDirectives();
		void	initializeValidLocationDirectives();
		bool	isDirectiveTerminatedCorrectly(const std::vector<std::string>& tokens, size_t start_index);
		void	validateBlockStructure(const std::vector<std::string>& tokens);
		void	validateDirectives(const std::vector<std::string>& tokens);
		void	validateLocationDirective(const std::string& directive, size_t index, const std::vector<std::string>& tokens);
		void	validateServerDirective(const std::string& directive, size_t index, const std::vector<std::string>& tokens);

	public:
		ConfigValidator();
		~ConfigValidator();

		void	validate(const std::vector<std::string>& tokens);
};

#endif
