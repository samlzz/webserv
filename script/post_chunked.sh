PORT=$1

(printf "POST /upload HTTP/1.1\r
Host: localhost:$PORT\r
Content-Type: application/octet-stream\r
Transfer-Encoding: chunked\r
\r
5\r
Ceci \r
7\r
est un \r
8\r
test de \r
e\r
message envoye\r
e\r
 par morceaux.\r
0\r
\r
") | nc localhost $PORT
