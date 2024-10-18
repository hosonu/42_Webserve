name parameters (spaces between them)
{} braces : server, location
;	end with semicolon

defaultでの設定値

*server定義：1n1p*
listen(port) : 80　{listen host}
host : localhost(lisenの中にある)
1.listen 127.0.0.1:8081;
2.listen 80;
3.lsiten 127.0.0.1;
4.listen localhost;
・listehostの分解(error chceck)
・hostの値確認(error chceck)
・全体のerror check

server_name : ""
1.error_check(どのようなpを得られるか確認)

*グローバル設定:1nmp,exceptCMBS*
error_page : (num or.html)

client_max_body_size : 1m
(the 413 (Request Entity Too Large) error is returned to the client)

