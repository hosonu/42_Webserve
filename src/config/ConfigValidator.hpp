#ifndef CONFIGVALIDATOR
#define CONFIGVALIDATOR

#include <vector>
#include <string>
#include <set>

class ConfigValidator {
	private:
		const std::set<std::string> valid_server_directives;
		const std::set<std::string> valid_location_directives;

		bool	isDirectiveTerminatedCorrectly(const std::vector<std::string>& tokens, size_t start_index);
		void	validateBlockStructure(const std::vector<std::string>& tokens);
		void	validateDirectives(const std::vector<std::string>& tokens);

	public:
		ConfigValidator();
		~ConfigValidator();

		void	validate(const std::vector<std::string>& tokens);
};

#endif
