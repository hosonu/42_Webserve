#include "core/Server.hpp"

int main(int argc, char **argv){
	
	Server s("127.0.0.1", 8080);
	s.run();
}
