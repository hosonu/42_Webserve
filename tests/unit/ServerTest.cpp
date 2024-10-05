#include <gtest/gtest.h>
#include "Server.hpp"

class ServerTest : public ::testing::Test {
protected:
    std::unique_ptr<Server> server;
    Config test_config;

    void SetUp() override {
        // テスト用の設定を作成
        test_config.addListener("127.0.0.1", 8083);
        server.reset(new Server(test_config));
    }
};

TEST_F(ServerTest, InitializationTest) {
    EXPECT_TRUE(server->initialize());
}

TEST_F(ServerTest, MultiplePortTest) {
    Config multi_port_config;
    multi_port_config.addListener("127.0.0.1", 8084);
    multi_port_config.addListener("127.0.0.1", 8085);
    
    Server server(multi_port_config);
    EXPECT_TRUE(server.initialize());
}