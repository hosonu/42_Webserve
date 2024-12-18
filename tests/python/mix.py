import requests

# ベースURL（テスト対象のサーバーURL）
url_base = "http://localhost:8080"

# テストするURLリスト
url_list = [
    f"{url_base}/",  # GETルート
    f"{url_base}/www/default/index.html",  # GETファイル取得
    f"{url_base}/upload",  # POSTリクエスト（アップロード）
    f"{url_base}/upload/uploaded_file_1734431603.txt",  # DELETEリクエスト（削除）
]

# 1. GETリクエストのテスト
def test_get_request(url):
    response = requests.get(url)
    print(f"GET {url}")
    print("Status Code:", response.status_code)
    print("Response Body:", response.text)
    print("="*60)

# 2. POSTリクエストのテスト
def test_post_request(url, data):
    response = requests.post(url, data=data)
    print(f"POST {url} with data {data}")
    print("Status Code:", response.status_code)
    print("Response Body:", response.text)
    print("="*60)


# 4. DELETEリクエストのテスト
def test_delete_request(url):
    response = requests.delete(url)
    print(f"DELETE {url}")
    print("Status Code:", response.status_code)
    print("Response Body:", response.text)
    print("="*60)

# テストデータ
post_data = {
    "name": "John Doe",
    "age": 30
}

put_data = {
    "name": "Jane Doe",
    "age": 25
}

# 各リクエストをテスト
#test_get_request(url_list[0])  # GETリクエスト：ルートURL
#test_get_request(url_list[1])  # GETリクエスト：ファイルURL
#test_post_request(url_list[2], post_data)  # POSTリクエスト：アップロード
test_delete_request(url_list[3])  # DELETEリクエスト：削除
