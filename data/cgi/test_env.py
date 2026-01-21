#!/usr/bin/env python3
import os

print("Content-Type: text/html")
print()
print("<html><body>")
print("<h1>CGI Environment Variables</h1>")
print("<table border='1'>")
print("<tr><th>Variable</th><th>Value</th></tr>")

cgi_vars = [
    'GATEWAY_INTERFACE', 'SERVER_NAME', 'SERVER_PORT',
    'SERVER_PROTOCOL', 'SERVER_SOFTWARE', 'REQUEST_METHOD',
    'PATH_INFO', 'PATH_TRANSLATED', 'SCRIPT_NAME',
    'QUERY_STRING', 'REMOTE_ADDR', 'REMOTE_HOST',
    'CONTENT_TYPE', 'CONTENT_LENGTH'
]

for var in cgi_vars:
    value = os.environ.get(var, '(not set)')
    print(f"<tr><td>{var}</td><td>{value}</td></tr>")

print("</table>")
print("</body></html>")