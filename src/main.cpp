#include "core/Server.hpp"
#include "config/Config.hpp"
#include <csignal>

Server* g_server = NULL;  // グローバル変数としてサーバーインスタンスへのポインタを保持
int g_signal = 0;

void signal_handler(int signum) {
	if (g_server) {
		g_server->closeServer();
	}
	g_signal = signum;
	throw std::runtime_error("Signal received: terminating...");
}

std::string getValidatePath(int argc, char *argv[]) {
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

int main(int argc, char *argv[]) {

    try {
		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);
		signal(SIGQUIT, signal_handler);
        std::string path = getValidatePath(argc, argv);
        Config conf;
        conf.loadConfigFile(path);
        Server s(conf);
        g_server = &s;
        
        s.setServer();
        s.run();
        
        g_server = NULL;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return g_signal;
    }
    
    return g_signal;
}
