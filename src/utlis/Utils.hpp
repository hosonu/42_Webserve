#ifndef UTILS_HPP
#define UTILS_HPP

#include "../core/Server.hpp"
#include <csignal>
#include <string>

extern Server* g_server;
extern int g_signal;

class Utils {
public:
    static std::string getValidatePath(int argc, char *argv[]);
	static void signal_handler(int signum);

	class SignalException : public std::exception {
	public:
		const char* what() const throw();
	};
};

#endif