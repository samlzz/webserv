PORT=$1

(printf "POST /test_upload.bin HTTP/1.1\r
Host: localhost:$PORT\r
Content-Type: application/octet-stream\r
Content-Length: %d\r
\r
" $(stat -c%s bin/data.bin); cat bin/data.bin) | nc localhost $PORT