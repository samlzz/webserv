PORT=$1

(printf "HEAD /index.html HTTP/1.1\r
Host: localhost:$PORT\r
\r
") | nc localhost $PORT