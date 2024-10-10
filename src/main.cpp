#include "core/Server.hpp"
#include "config/Config.hpp"

int main(int argc, char **argv){
	Config conf;
	if (argc == 2)
		conf.parse(argv[1]);
	
	// std::cout << "Host: "<< conf.Servers[0].getHost() <<std::endl;
	// std::cout <<  "Port: " << conf.Servers[0].getListenPort() << std::endl;

	try {
		Server s(conf);
		s.setServer();

		s.run();
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
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

	Server s("127.0.0.1", 8080);
	s.run();
}

// int main(int argc, char **argv){
// 	Config conf;
// 	if (argc == 2)
// 		conf.parse(argv[1]);
	
// 	// std::cout << "Host: "<< conf.Servers[0].getHost() <<std::endl;
// 	// std::cout <<  "Port: " << conf.Servers[0].getListenPort() << std::endl;

// 	try {
// 		std::vector<ServerConfig> configs = conf.getServerConfig();

// 		Server s(configs[0].host, configs[0].listenPort);
// 		s.run();
// 	} catch (std::exception &e) {
// 		std::cout << e.what() << std::endl;
// 	}
// }

