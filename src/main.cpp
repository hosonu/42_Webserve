#include "core/Server.hpp"
#include "config/Config.hpp"

bool checkArgv(int argc, char *argv[]) {
	if (argc == 1) {
		return 1;
	 } else if(argc != 2) {
		std::cerr << "ERROR: too many arguments" << std::endl;
		return false;
	 }
	 return true;
}

static std::string	getPath(int argc, char *argv[]) {
	std::string path;

	if (argc == 1) {
		path = "../config/default.conf";
	} else if (argc == 2) {
		path = std::string(argv[1]);
	}
	return path;
}

int main(int argc, char *argv[]){

	if (checkArgv(argc, argv) == false) {
		return false;
	}
	std::string path(getPath(argc, argv));

	Config conf;
	conf.parse(path);
	try {
		Server s(conf);
		s.setServer();

		s.run();
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
