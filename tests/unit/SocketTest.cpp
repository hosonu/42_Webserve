#include <gtest/gtest.h>
#include "Socket.hpp"

// class SocketTest : public ::testing::Test {
// protected:
//     void SetUp() override {
//         // テストごとの初期化
//     }

//     void TearDown() override {
//         // テストごとのクリーンアップ
//     }
// };

// 基本的な初期化テスト
TEST_F(SocketTest, InitializationTest) {
    Socket socket("127.0.0.1", 8080);
    // EXPECT_TRUE(socket.isValid());
    EXPECT_EQ(socket.getPort(), 8080);
    EXPECT_EQ(socket.getHost(), "127.0.0.1");
}

// バインドテスト
TEST_F(SocketTest, BindTest) {
    Socket socket("127.0.0.1", 8081);
    EXPECT_TRUE(socket.bind());
    // 同じポートへの2重バインドは失敗するはず
    Socket socket2("127.0.0.1", 8081);
    EXPECT_FALSE(socket2.bind());
}

// 非ブロッキング設定テスト
TEST_F(SocketTest, NonBlockingTest) {
    Socket socket("127.0.0.1", 8082);
    EXPECT_TRUE(socket.setNonBlocking());
    // 非ブロッキングモードの検証
    // (実際の実装では、getFdのフラグをチェックする)
}