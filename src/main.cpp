#include "core/Server.hpp"
#include "config/Config.hpp"
#include <csignal>

int	getValidatePath(int argc, char *argv[], std::string &path) {
	
	if (argc == 1) {
		path = "config/test.conf";
	}  else if (argc == 2) {
		path = std::string(argv[1]);
	} else {
		std::cerr << "ERROR: too many arguments" << std::endl;
		return false;
	}
	if (path.size() >= 5 && path.substr(path.size() - 5) == ".conf") {
		return true;
	} else {
		std::cerr << "ERROR: The extension must be \".conf\".\n";
        return false;
	}
}

bool ignore_sigpipe()
{
	if (std::signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return (false);
	return (true);
}

int main(int argc, char *argv[]){
	
	std::string path;

	if (!(ignore_sigpipe()))
		return (1);
	if (getValidatePath(argc, argv, path) == false) {
		return 1;
	}

	try {
		Config conf;
		conf.loadConfigFile(path);
		Server s(conf);
		s.setServer();
		s.run();
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
