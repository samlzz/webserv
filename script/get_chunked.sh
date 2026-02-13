PORT=$1

(printf "GET /index.html HTTP/1.1\r
Host: localhost:$PORT\r
Transfer-Encoding: chunked\r
\r
0\r
\r
") | nc localhost $PORT