#include "core/Server.hpp"
#include "config/Config.hpp"

int main(int argc, char **argv){
	Config conf;
	if (argc == 2)
		conf.parse(argv[1]);
	
	std::cout << "Host: "<< conf.Servers[10].getHost() <<std::endl;
	std::cout <<  "Port: " << conf.Servers[10].getListenPort() << std::endl; 
	Server s(conf.Servers[10].getHost(), conf.Servers[10].getListenPort()); 
	// Server s("127.0.0.1", 8080);
	s.run();
}

// int main() {
// 	ServerConfig sc;
// 	sc.getHost();
// }
