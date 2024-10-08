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

// int main() {
// 	ServerConfig sc;
// 	sc.getHost();
// }
