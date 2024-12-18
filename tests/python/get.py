import requests

# テストするURLリスト
url_list = [
    'http://localhost:8080',
    'http://localhost:8080/src/',
    'http://localhost:8080/www/default/tmp.html',
    'http://localhost:8080/www/404.html',
]

# HTTPメソッドの種類を指定
methods = ['GET', 'POST', 'DELETE']

# POSTやPUTの際に送信するデータ（必要に応じて変更）
payload = {'key': 'value'}

for url in url_list:
    for method in methods:
        try:
            # HTTPリクエストの送信
            if method == 'GET':
                response = requests.get(url)
            elif method == 'POST':
                response = requests.post(url, data=payload)
            elif method == 'DELETE':
                response = requests.delete(url)
            else:
                print(f"Unsupported method: {method}")
                continue

            # リクエスト情報の表示
            print("--------------------------------------------------------------")
            print("HTTP Method:", method)
            print("Request URL:", response.request.url)
            print("Request Headers:")
            for key, value in response.request.headers.items():
                print(f"  {key}: {value}")
            print("Request Body:", response.request.body)
            print("--------------------------------------------------------------")

            # レスポンス情報の表示
            print("Status Code:", response.status_code)
            print("Response Headers:")
            for key, value in response.headers.items():
                print(f"  {key}: {value}")
            print("Response Body:")
            print(response.text)
            print("==============================================================")

        except requests.exceptions.RequestException as e:
            # エラー時の表示
            print(f"An error occurred with URL: {url}, Method: {method}")
            print(f"Error: {e}")
            print("==============================================================")
