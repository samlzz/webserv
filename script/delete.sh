PORT=$1

(printf "DELETE /test.txt HTTP/1.1\r
Host: localhost:$PORT\r
Content-Length: 0\r
\r
") | nc localhost $PORT