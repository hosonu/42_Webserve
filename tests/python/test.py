import socket
import time


def send_request(request, host, port):
    """サーバーにリクエストを送信してレスポンスを取得する"""
    print("=== Sending Request ===")
    print(request)
    print("=======================\n")
    
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            s.connect((host, port))
            s.sendall(request.encode())
            response = s.recv(4096)
            print("=== Received Response ===")
            print(response.decode())
            print("=========================\n")
        except Exception as e:
            print(f"Error: {e}\n")


def main():
    host = "localhost"  # テスト対象のホスト
    port = 8080         # テスト対象のポート

    # 1. 正常なGETリクエスト
    get_request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    send_request(get_request, host, port)

    # 2. 正常なPOSTリクエスト（ボディあり）
    post_request = (
        "POST /upload HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 13\r\n\r\n"
        "key=value123"
    )
    send_request(post_request, host, port)

    # 3. 正常なDELETEリクエスト
    delete_request = "DELETE /file/to/delete HTTP/1.1\r\nHost: localhost\r\n\r\n"
    send_request(delete_request, host, port)

    # 4. 欠如したHostヘッダー
    incomplete_header = "GET / HTTP/1.1\r\n\r\n"
    send_request(incomplete_header, host, port)

    # 5. 不正なHTTPメソッド
    invalid_method = "FOO / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    send_request(invalid_method, host, port)

    # 6. 異常に長いリクエスト行
    long_uri = "GET /" + "A" * 10000 + " HTTP/1.1\r\nHost: localhost\r\n\r\n"
    send_request(long_uri, host, port)

    # 7. 不完全なリクエスト
    partial_request = "POST /upload HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10\r\n\r\nkey="
    print("=== Sending Partial Request ===")
    print(partial_request)
    print("===============================\n")
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            s.connect((host, port))
            s.sendall(partial_request.encode())
            time.sleep(1)  # サーバーの応答を待たずに接続を切断
            response = s.recv(4096)
            print("=== Received Response ===")
            print(response.decode())
            print("=========================\n")
        except Exception as e:
            print(f"Error: {e}\n")

    # 8. 不正なHTTPバージョン
    invalid_version = "GET / HTTP/3.0\r\nHost: localhost\r\n\r\n"
    send_request(invalid_version, host, port)

    # 9. 存在しないパスへのGETリクエスト
    non_existent_path = "GET /nonexistent/path HTTP/1.1\r\nHost: localhost\r\n\r\n"
    send_request(non_existent_path, host, port)


if __name__ == "__main__":
    main()
