#include "core/Server.hpp"
#include "config/Config.hpp"
#include "utlis/Utils.hpp"

Server* g_server = NULL;
int g_signal = 0;

int main(int argc, char *argv[]) {
    try {
		signal(SIGINT, Utils::signal_handler);
        std::string path = Utils::getValidatePath(argc, argv);
        Config conf;
        conf.loadConfigFile(path);
        Server s(conf);
        g_server = &s;
        
        s.setServer();
        s.run();
        
        g_server = NULL;

    } catch (Utils::SignalException &e) {
		std::cerr << e.what() << std::endl;
        return g_signal;
	} catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
		return 1;
    }
    return 0;
}
