#!/usr/bin/env python3
import time

print("Content-Type: text/plain\n")

time.sleep(65)

print("Cette ligne ne devrait jamais s'afficher si le serveur a un timeout CGI")