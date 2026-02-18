#!/usr/bin/env python3

print("Content-Type: text/html\n")

# Division par zéro pour provoquer un crash
result = 1 / 0

print("<html>")
print("<body>")
print("<h1>Cette page ne devrait jamais s'afficher</h1>")
print("</body>")
print("</html>")