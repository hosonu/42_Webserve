#include "core/Server.hpp"
#include "config/Config.hpp"

int main(int argc, char **argv){
	Config conf;
	if (argc == 2)
		conf.parse(argv[1]);
	
	// std::cout << "Host: "<< conf.Servers[0].getHost() <<std::endl;
	// std::cout <<  "Port: " << conf.Servers[0].getListenPort() << std::endl;

	try {
		Server s(conf.getServerConfig());
		s.run();
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
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

