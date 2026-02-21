PORT=$1
DELAY=${2:-1}

(printf "POST /upload HTTP/1.1\r
Host: localhost:$PORT\r
Content-Type: text/plain\r
Transfer-Encoding: chunked\r
\r
"
sleep $DELAY
printf "5\r
Ceci \r
"
sleep $DELAY
printf "7\r
est un \r
"
sleep $DELAY
printf "8\r
test de \r
"
sleep $DELAY
printf "e\r
message envoye\r
"
sleep $DELAY
printf "e\r
 par morceaux.\r
"
sleep $DELAY
printf "0\r
\r
"
) | nc localhost $PORT