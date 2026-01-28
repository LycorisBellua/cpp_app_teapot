# Teapot

This project has been created as part of a school curriculum by 
[Lycoris Bellua](https://github.com/LycorisBellua) and 
[Joshw34](https://github.com/joshw34).  

## Description

**Teapot** is a small HTTP/1.1 web server written in C++98. It takes a 
configuration file as argument, which describes a server whose role is to serve 
a static website.  

It handles:  
- GET, HEAD, POST and DELETE methods.
- Fixed-length request bodies.
- Chunked request bodies.
- CGI scripts (e.g.: Python, PHP, JS...).
- Cookies, with the simple example of giving each client session a unique 
background color.

Certain HTTP/1.1 features are considered out of scope, such as virtual 
hosting.  

<details>
<summary>About virtual hosting</summary>

Virtual hosting allows the same IP and port to be used for different domains. 
When the client sends a request, it indicates the target domain, and the server 
knows which specific website to serve. If the IP is used instead of a domain, 
then the first domain of the list becomes the target.  

Because we didn't implement this feature, each IP/port pair serves only one 
website. They also don't have a domain name because it's not relevant to our 
use case.  
</details>

<details>
<summary>What's a cookie?</summary>

A cookie is a small piece of data that a web server generates and sends to the 
client within a response. The client is then meant to hold onto the cookie, and 
to send it back with any request. If relevant, the server can tell the client 
to delete or update the cookie, changing whatever data it contains. It's a tool 
that allows HTTP, which is a stateless protocol, to have session management. In 
other words: as long as the client holds onto the cookies, the server can use 
the cookie feature to remember things about this client, and to adopt a 
specific behavior depending on how this information is used.  

The data that a cookie contains is:  
- a key-value string pair (e.g. `hello=world`),
- a domain pattern (e.g. if value is `example.com`, then the client sends the 
cookie if the site domain is `example.com`, `www.example.com` or even 
`shop.example.com`),
- a path pattern (e.g. if the value is `/` then all paths are eligible, but if 
the value is `/blog/` then `/blog/` and `/blog/lorem-ipsum` work, but not 
`/shop`),
- and a life cycle (max age in seconds or an expiration date).

The server doesn't have to specify anything other than the key-value pair. If 
the domain or the path aren't given, they aren't patterns anymore but precise 
values, and these values are derived from the domain or the path who were 
targeted by the request. As for the life cycle, if not provided then the cookie 
is only kept while the session is alive, aka until the browser closes.  
</details>

<details>
<summary>About our cookies</summary>

The cookie feature uses a simple example: a background color for the HTML 
pages. Each server block has its own cookie list to check against, and if a 
request cookie isn't recognized, the response tells the client to delete it.  

A special case to mention is the browser accessing the `localhost` or 
`127.0.0.1` domains:  
- On the server-side, because we didn't implement virtual hosting, `localhost` 
is interpreted in its IP form. This means that, whether the client tries to 
reach `localhost` or `127.0.0.1`, we peruse the same cookie list.
- On the client-side, when the client receives the cookie, because we didn't 
give the cookie a domain value, the client uses the domain it sent the 
request to. This leads to the client marking the cookie as belonging to one 
domain or the other. Therefore, `localhost` and the IP form are two different 
sessions.
</details>

## Instructions

**Teapot** is a Linux-only project. Here is how to compile and run it:  

```
git clone https://github.com/LycorisBellua/cpp_app_webserv.git
cd cpp_app_webserv
make
./webserv configs/default.conf
```

While the server is running, you can use a client to send requests. This can be 
another CLI window, or a browser.  

Here are a few CLI examples to get the index page:
- `curl -v --http1.1 -X GET http://localhost:8080/`
- `printf "GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n" | nc localhost 8080`

In the browser of your choice, type in any of these URLs:
- `http://localhost:8080/`
- `http://127.0.0.1:8080/`
- `http://0.0.0.0:8080/`

Besides the index, here are the relevant routes:
- TODO: List the routes.

## Resources

For learning, we relied on the [MDN Web Docs](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status/418) 
and on the RFCs:  
- [RFC 9110 - HTTP Semantics](https://datatracker.ietf.org/doc/html/rfc9110)
- [RFC 9112 - HTTP/1.1](https://datatracker.ietf.org/doc/html/rfc9112)
- [RFC 3986 - URI Syntax](https://datatracker.ietf.org/doc/html/rfc3986)
- [RFC 6265 - Cookies](https://datatracker.ietf.org/doc/html/rfc6265)
- [RFC 3875 - CGI 1.1](https://datatracker.ietf.org/doc/html/rfc3875)

AI was also used to generate the example website and CGI scripts.  
