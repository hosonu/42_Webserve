#!/usr/bin/env python3
import sys

#try:
#    while True:  # 無限ループ
#        print("Content-Type: text/html\r\n\r\n", end='')  # ヘッダーを出力
#except BrokenPipeError:
#    sys.exit(0)  # クライアントが切断されても正常に終了する


while True:  # 無限ループ
    print("Content-Type: text/html\r\n\r\n", end='')  # ヘッダーを出力