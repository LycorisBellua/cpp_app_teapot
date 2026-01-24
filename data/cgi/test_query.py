#!/usr/bin/env python3
import os
from urllib.parse import parse_qs

query_string = os.environ.get('QUERY_STRING', '')
params = parse_qs(query_string)

print("Content-Type: text/html")
print()
print("<html><body>")
print("<h1>Query String Test</h1>")
print(f"<p>Raw query: {query_string}</p>")
print("<h2>Parsed Parameters:</h2>")
print("<ul>")
for key, values in params.items():
    for value in values:
        print(f"<li>{key} = {value}</li>")
print("</ul>")
print("</body></html>")