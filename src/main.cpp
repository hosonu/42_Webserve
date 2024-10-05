#include "core/Server.hpp"

int main(){
	Server s("127.0.0.1", 8080);
	s.run();
}
