#!/usr/bin/env python3

import cgi

# フォームデータを取得
#form = cgi.FieldStorage()
#name = form.getvalue("name", "Unknown")  # "name"フィールドがない場合はデフォルト値を設定
#age = form.getvalue("age", "Unknown")    # "age"フィールドがない場合はデフォルト値を設定

# HTTPレスポンスヘッダーを出力
print("Content-Type: text/html\r\n\r\n")

# レスポンスボディを出力
print(f"""
<html>
    <body>
        <h1>Hello, kmotoyam!</h1>
        <p>Age: 25</p>
    </body>
</html>
""")
