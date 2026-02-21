#!/usr/bin/env python3

import os

username = os.environ.get('SESSION_USERNAME', 'Guest')

poem = f"""À {username} je dédie ces vers,
Toi qui traverses l'univers,
Ton nom résonne comme un écho,
Dans les étoiles et les nuageaux.

{username}, voyageur de l'instant,
Chaque pas que tu fais est brillant,
Le monde s'éveille à ton passage,
Et le soleil salue ton visage.

Alors avance, sans jamais douter,
Car {username} est fait pour briller."""

print("Content-Type: text/html; charset=UTF-8\r\n\r\n")
print("<html>")
print("<head><style>")
print("  body { font-family: Georgia, serif; background: #1a1a2e; color: #e0e0e0; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }")
print("  .poem { background: #16213e; border-left: 4px solid #e94560; padding: 2em 3em; border-radius: 8px; max-width: 500px; }")
print("  h1 { color: #e94560; font-size: 1.4em; margin-bottom: 1em; }")
print("  pre { white-space: pre-wrap; line-height: 1.8; font-size: 1.05em; }")
print("  a { color: #e94560; text-decoration: none; font-size: 0.9em; }")
print("</style></head>")
print("<body><div class='poem'>")
print(f"<h1>Poème pour {username}</h1>")
print(f"<pre>{poem}</pre>")
print("<br><a href='/'>← Retour</a>")
print("</div></body></html>")