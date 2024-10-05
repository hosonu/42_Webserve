class ServerIntegrationTest : public ::testing::Test {
protected:
    std::unique_ptr<Server> server;
    std::unique_ptr<Socket> client;

    void SetUp() override {
        // サーバーの設定と起動
        Config config;
        config.addListener("127.0.0.1", 8086);
        server.reset(new Server(config));
        ASSERT_TRUE(server->initialize());
        
        // 別スレッドでサーバーを起動
        server_thread = std::thread([this]() {
            server->run();
        });
        
        // クライアントの準備
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        client.reset(new Socket("127.0.0.1", 8086));
    }

    void TearDown() override {
        server->stop();
        if (server_thread.joinable()) {
            server_thread.join();
        }
    }

private:
    std::thread server_thread;
};

TEST_F(ServerIntegrationTest, BasicConnectionTest) {
    EXPECT_TRUE(client->connect());
    // 基本的な接続テスト
}