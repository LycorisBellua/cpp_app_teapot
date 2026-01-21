#!/usr/bin/env python3

print("Content-Type: text/html")
print("X-Custom-Header: MyValue")
print("Cache-Control: no-cache")
print("Set-Cookie: session=abc123; Path=/")
print()
print("<html><body>")
print("<h1>Custom Headers Test</h1>")
print("<p>Check response headers in browser dev tools.</p>")
print("</body></html>")