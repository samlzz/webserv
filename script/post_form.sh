PORT=$1

(printf "POST /upload.bin HTTP/1.1\r
Host: localhost:$PORT\r
Content-Type: application/x-www-form-urlencoded\r
Content-Length: %d\r
\r
" $(stat -c%s bin/form.bin); cat bin/form.bin) | nc localhost $PORT