PORT=$1
TIMEOUT=${2:-30}  # Temps d'attente par défaut: 30 secondes

(printf "POST /upload HTTP/1.1\r
Host: localhost:$PORT\r
Content-Type: text/plain\r
Transfer-Encoding: chunked\r
\r
4\r
Ceci\r
7\r
 est un\r
8\r
 test de\r
"
sleep $TIMEOUT
) | nc localhost $PORT