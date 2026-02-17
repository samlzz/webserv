#!/usr/bin/env python3
import os
import sys

env_vars = dict(os.environ)

# Générer le contenu HTML
html_content = """<!DOCTYPE html>
<html lang="fr">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Variables d'environnement CGI</title>
</head>
<body>
	<div class="container">
		<h1>Variables d'environnement CGI</h1>
		
		<table id="envTable">
			<thead>
				<tr>
					<th>Variable</th>
					<th>Valeur</th>
				</tr>
"""

for key in sorted(env_vars.keys()):
	value = env_vars[key]
	if value == "":
		value_display = '<span class="empty">(vide)</span>'
	else:
		# Échapper les caractères HTML
		value_escaped = value.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;').replace('"', '&quot;')
		value_display = value_escaped
	
	html_content += f"""                <tr>
					<td class="var-name">{key}</td>
					<td class="var-value">{value_display}</td>
				</tr>
"""

html_content += """
	</div>
</body>
</html>
"""

content_length = len(html_content.encode('utf-8'))

print("Content-Type: text/html; charset=utf-8")
print(f"Content-Length: {content_length}")
print()  #\r\n

print(html_content, end='')
