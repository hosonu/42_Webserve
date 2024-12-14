#!/usr/bin/env python3

import cgi

# フォームデータを取得
#form = cgi.FieldStorage()
#name = form.getvalue("name", "Unknown")  # "name"フィールドがない場合はデフォルト値を設定
#age = form.getvalue("age", "Unknown")    # "age"フィールドがない場合はデフォルト値を設定

# HTTPレスポンスヘッダーを出力
while(1)
 print("Content-Type: text/html\r\n\r\n")
