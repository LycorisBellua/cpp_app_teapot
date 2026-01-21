#!/usr/bin/env python3

# No Status header - should default to 200 OK
print("Content-Type: text/plain")
print()
print("<html><body>")
print("<h1>No Status Check</h1>")
print("<p>This should return 200 OK by default</p>")
print("</body></html>")