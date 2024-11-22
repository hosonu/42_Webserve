#include "ConfigValidator.hpp"

ConfigValidator::ConfigValidator() {
	initializeValidServerDirectives();
	initializeValidLocationDirectives();
}
ConfigValidator::~ConfigValidator() {
    // Destructor implementation (if needed)
}


void	ConfigValidator::initializeValidServerDirectives() {
	valid_server_directives.insert("listen");
	valid_server_directives.insert("server_name");
	valid_server_directives.insert("error_page");
	valid_server_directives.insert("client_max_body_size");
}

void	ConfigValidator::initializeValidLocationDirectives() {
	valid_location_directives.insert("allow_methods");
	valid_location_directives.insert("autoindex");
	valid_location_directives.insert("root");
	valid_location_directives.insert("index");
	valid_location_directives.insert("return");
	//valid_location_directives.insert("");
}

void	ConfigValidator::validateBlockStructure(const std::vector<std::string>& tokens) {
	int brace_count = 0;
	bool in_server_block = false;
	bool in_location_block = false;

	if (tokens[0] != "server") {
		throw std::runtime_error("[emerg] syntax error");
	}
	for (size_t i = 0; i < tokens.size(); ++i) {
		if (tokens[i] == "{") {
			brace_count++;
			if (tokens[i-1] == "server") {
				if (in_server_block) 
					throw std::runtime_error("Nested server blocks are not allowed");
				in_server_block = true;
			}
			if (tokens[i-1] == "location") {
				if (!in_server_block)
					throw std::runtime_error("Location block must be inside a server block");
				if (in_location_block)
					throw std::runtime_error("Nested location blocks are not allowed");
				in_location_block = true;
			}
		}
		else if (tokens[i] == "}") {
			brace_count--;
			if (brace_count < 0)
				throw std::runtime_error("Unbalanced braces in configuration");
			
			if (in_location_block) in_location_block = false;
			if (in_server_block) in_server_block = false;
		}
	}

	if (brace_count != 0)
		throw std::runtime_error("Unbalanced braces in configuration");
}


void	ConfigValidator::validateDirectives(const std::vector<std::string>& tokens) {
	std::string current_context = "";

	for (size_t i = 0; i < tokens.size(); ++i) {
		if (tokens[i] == "server") {
			current_context = "server";
			continue;
		}
		if (tokens[i] == "location") {
			current_context = "location";
			continue;
		} else if (tokens[i] == "{" || tokens[i] == "}") {
			continue;
		}
		else if (current_context == "server") {
			validateServerDirective(tokens[i], i, tokens);
		}
		else if (current_context == "location") {
			validateLocationDirective(tokens[i], i, tokens);
		}
	}
}

bool	ConfigValidator::isDirectiveTerminatedCorrectly(const std::vector<std::string>& tokens, size_t start_index) {

	for (size_t i = start_index; i < tokens.size(); ++i) {
		if (tokens[i] == ";") return true;
		if (tokens[i] == "{" || tokens[i] == "}") return false;
	}
	return false;
}

void	ConfigValidator::validateServerDirective(const std::string& directive, size_t index, const std::vector<std::string>& tokens) {
	if (valid_server_directives.count(directive) > 0) {
		if (!isDirectiveTerminatedCorrectly(tokens, index)) {
			throw std::runtime_error("Invalid termination for server directive: " + directive);
		}
	}
}

void	ConfigValidator::validateLocationDirective(const std::string& directive, size_t index, const std::vector<std::string>& tokens) {
	if (valid_location_directives.count(directive) > 0) {
		if (!isDirectiveTerminatedCorrectly(tokens, index)) {
			throw std::runtime_error("Invalid termination for location directive: " + directive);
		}
	}
}

void	ConfigValidator::validate(const std::vector<std::string>& tokens) {
	validateBlockStructure(tokens);
	validateDirectives(tokens);
}
