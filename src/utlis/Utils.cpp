#include "Utils.hpp"

std::string Utils::getValidatePath(int argc, char *argv[]) {
    std::string path;
    if (argc == 1) {
        path = "config/test.conf";
    } else if (argc == 2) {
        path = std::string(argv[1]);
    } else {
        throw std::runtime_error("ERROR: too many arguments");
    }
    if (path.size() >= 5 && path.substr(path.size() - 5) == ".conf") {
        return path;
    } else {
        throw std::runtime_error("ERROR: The extension must be \".conf\"");
    }
}

void Utils::signal_handler(int signum) {
	if (g_server) {
		g_server->closeServer();
	}
	g_signal = signum;
	throw Utils::SignalException();
}

const char* Utils::SignalException::what() const throw() {
	return "Terminating...";
}
