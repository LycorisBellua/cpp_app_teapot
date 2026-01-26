# Teapot

This project is a small HTTP/1.1 web server written with C++98.  

## TODO

- List features we do implement.  

## Virtual hosting

Virtual hosting allows the same IP and port to be used by different domains. 
When the client sends a request, it indicates the target domain, and the server 
knows which specific website to serve. If the IP is used instead of a domain, 
then the first domain of the list becomes the target.  

We didn't implement virtual hosting. Each IP/port pair serves only one 
website. They also don't have a domain name because it's not relevant to our 
use case.  

## What's a cookie?

A cookie is a small piece of data that a web server generates and sends to the 
client within a response. The client is then meant to hold onto the cookie, and 
to send it back with any request. If relevant, the server can tell the client 
to delete or update the cookie, changing whatever data it contains. It's a tool 
that allows HTTP, which is a stateless protocol, to have session management. In 
other words: as long as the client holds onto the cookies, the server can use 
the cookie feature to remember things about this client, and to adopt a 
specific behavior depending on how this information is used.  

The data that a cookie contains is:  
- a key-value string pair (e.g. "hello=world"),  
- a domain pattern (e.g. if value is "example.com", then the client sends the 
cookie if the site domain is "example.com", "www.example.com" or even 
"shop.example.com"),  
- a path pattern (e.g. if the value is "/" then all paths are eligible, but if 
the value is "/blog/" then "/blog/" and "/blog/lorem-ipsum" work, but not 
"/shop"),  
- and a life cycle (max age in seconds or an expiration date).  

The server doesn't have to specify anything other than the key-value pair. If 
the domain or the path aren't given, they aren't patterns anymore but precise 
values, and these values are derived from the domain or the path who were 
targeted by the request. As for the life cycle, if not provided then the cookie 
is only kept while the session is alive, aka until the browser closes.  

## About our cookies

The cookie feature uses a simple example: a background color for the HTML 
pages. Each server block has its own cookie list to check against, and if a 
request cookie isn't recognized, the response tells the client to delete it.  

A special case to mention is the browser accessing the "localhost" or 
"127.0.0.1" domains:  
- On the server-side, because we didn't implement virtual hosting, "localhost" 
is interpreted in its IP form. This means that, whether the client tries to 
reach "localhost" or "127.0.0.1", we peruse the same cookie list.  
- On the client-side, when the client receives the cookie, because we didn't 
give the cookie a domain value, the client uses the domain it sent the 
request to. This leads to the client marking the cookie as belonging to one 
domain or the other. Therefore, "localhost" and the IP form are two different 
sessions.  
