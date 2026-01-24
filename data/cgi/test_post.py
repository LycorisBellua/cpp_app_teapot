#!/usr/bin/env python3
import sys
import os

content_length = int(os.environ.get('CONTENT_LENGTH', 0))
post_data = sys.stdin.read(content_length) if content_length > 0 else ""

print("Content-Type: text/html")
print()
print("<html><body>")
print("<h1>POST Data Received</h1>")
print(f"<p>Content-Length: {content_length}</p>")
print(f"<p>Content-Type: {os.environ.get('CONTENT_TYPE', 'Not set')}</p>")
print("<pre>")
print(post_data)
print("</pre>")
print("</body></html>")