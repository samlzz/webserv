# WEBSERV

This project has been created as part of the 42 curriculum by sliziard, achu and kevwang.

## Description

Webserv is a HTTP/1.1 web server implemented in C++98 as part of the 42 curriculum. It handles real HTTP traffic using a non-blocking, event-driven architecture based on the Reactor pattern (`poll()`). Its configuration syntax is inspired by NGINX and parsed through a custom PEG grammar.

---

##  Instructions

### Prerequisites
- C++ compiler supporting C++98
- `make`
- POSIX system (Linux)

### Commands

```bash
make          # Build the webserv binary (+ dependencies)
make clean    # Remove object files
make fclean   # Remove objects + binary
make re       # Full rebuild
```

The `ftpp` and `ft_log` libraries are built automatically.

---

##  Usage

```bash
# Build the project
make

# Run the server with a configuration file
./webserv assets/webserv.conf

# Example configurations
./webserv assets/exemples/dos.conf
./webserv assets/exemples/www_test.conf
```

## Features

### HTTP Methods
- `GET`      Serve static files
- `HEAD`     Same as GET, without the response body
- `POST`     File uploads, form submissions, CGI
- `PUT`      File upload (binary/chunked)
- `DELETE`   Delete files on the server

### Features
- HTTP/1.1 compliant — Keep-alive, full status code support
- Chunked Transfer-Encoding — Full parsing of chunked request bodies
- `location` blocks — Longest-prefix matching routing
- Autoindex — Automatic directory listing
- Redirections — `return` directive with status code + URL
- Custom error pages — Per status code, with a default page
- Client max body size — Configurable per server
- File uploads — Via multipart, `application/octet-stream`, `text/plain`
- Sessions & Cookies — Cookie-based session management
- MIME type detection — HTML, CSS, JS, JSON, XML, PDF, PNG, JPG, etc.

### CGI
- Extension-to-interpreter mapping (e.g., `.py → /usr/bin/python3`)
- Tested languages: Python, Perl, Ruby, PHP
- Non-blocking CGI I/O with timeout
- Full CGI environment variables (`REQUEST_METHOD`, `QUERY_STRING`, `PATH_INFO`, etc.)

## Configuration
The configuration file uses an NGINX-inspired syntax, parsed via a PEG grammar (`assets/webserv.peg`).

### Server Directives

| Directive              | Syntax                               | Default          | Description                         |
|------------------------|--------------------------------------|------------------|-------------------------------------|
| `listen`               | `listen port;`                       | `8080`           | Bind port                           |
| `root`                 | `root /path;`                        | —                | Document root directory             |
| `index`                | `index file;`                        | —                | Default index file                  |
| `autoindex`            | `autoindex on\|off;`                 | `off`            | Directory listing                   |
| `methods`              | `methods GET POST ...;`              | `GET POST DELETE` | Allowed HTTP methods               |
| `error_page`           | `error_page code /page.html;`        | `/errors/default.html` | Custom error pages            |
| `client_max_body_size` | `client_max_body_size 10M;`          | `1M`             | Max body size (K/M/G suffixes)      |
| `cgi`                  | `cgi .ext /path/to/interpreter;`     | —                | CGI extension → interpreter mapping |
| `return`               | `return [code] url;`                 | —                | HTTP redirection                    |

### Additional Directives (in `location` blocks)

| Directive        | Syntax                      | Description                          |
|------------------|-----------------------------|--------------------------------------|
| `upload`         | `upload /path;`             | Upload directory for POST/PUT        |
| `session_login`  | `session_login on\|off;`    | Enable session-based login           |
| `set_cookie`     | `set_cookie name;`          | Set a cookie from query parameter    |
| `vary_cookie`    | `vary_cookie name;`         | Vary response based on a cookie      |

### Configuration Example

```
server {
	listen 8001;                        # listening port, mandatory parameter
	error_page 404 /error/404.html;     # default error page
	client_max_body_size 1024;          # max request body size in bytes
	root docs/fusion_web/;              # root folder of site directory, full or relative path, mandatory parameter
	index index.html;                   # default page when requesting a directory, index.html by default

	location /tours {                   
		root docs/fusion_web;           # root folder of the location, if not specified, taken from the server. 
										# EX: - URI /tours           --> docs/fusion_web/tours
										#     - URI /tours/page.html --> docs/fusion_web/tours/page.html 
		autoindex on;                   # turn on/off directory listing
		methods GET HEAD POST DELETE;             # Allowed HTTP methods
													# Supported: GET, HEAD, POST, PUT, DELETE
													# allowed methods in location, GET only by default
		index index.html;               # default page when requesting a directory, copies root index by default
		return abc/index1.html;         # redirection
	}

	location /cgi-bin {
			methods GET POST;
			cgi .py /usr/bin/python3;                 # Map file extension to interpreter path
			cgi .pl /usr/bin/perl;                    # Multiple cgi directives allowed
			cgi .rb /usr/bin/ruby;                    # Extension format: .ext (e.g. .py, .pl, .sh)
			cgi .php /usr/bin/php;
			autoindex on;
		}

	location /uploads {
			methods GET POST PUT DELETE;
			upload html/www/uploads;                  # Directory where uploaded files are stored
			autoindex on;        
		}

	#redirection
	location /old-page {
		return 301 /new-page;                     # Redirect with status code + target URL
												# Status code is optional (defaults to 302)
												# Supports internal paths and external URLs
												# Examples: return 301 /index.html;
												#           return 302 https://example.com/;
												#           return /other-page;
	}  

	# Session & Cookie management
	location /dashboard {
		methods GET POST;
		session_login on;                         # Enable cookie-based session login (on/off)
		set_cookie theme;                         # Set a cookie from a query parameter
												# e.g. GET /dashboard?theme=dark sets cookie "theme=dark"
		vary_cookie theme;                        # Vary the response based on a cookie value
												# Useful for serving different content per user preference
	}
}
```

---

## Libraries

### ftpp — PEG Parsing Framework
- Author: sliziard
- Generic PEG parser producing structured ASTs
- Used to parse configuration files via the `assets/webserv.peg` grammar

### ft_log — Logging Library
- Author: sliziard
- Lightweight logging with configurable levels, ANSI colors, RAII scope tracing
- Zero-overhead when disabled (null-stream)

---

## Tests

Test scripts using `nc` (netcat) are available in `script/`:

| Script                     | Description                                         |
|----------------------------|-----------------------------------------------------|
| `get_chunked.sh`           | GET request with chunked transfer-encoding          |
| `head.sh`                  | HEAD request                                        |
| `post.sh`                  | Binary POST with Content-Length                     |
| `post_form.sh`             | POST with `application/x-www-form-urlencoded`       |
| `post_chunked.sh`          | POST with chunked transfer-encoding                 |
| `post_chunked_slow.sh`     | Chunked POST with delay between chunks              |
| `post_chunked_timeout.sh`  | Chunked POST triggering a request timeout           |
| `put.sh`                   | Binary PUT                                          |
| `put_chunked.sh`           | PUT with chunked transfer-encoding                  |
| `delete.sh`                | DELETE with Content-Length: 0                        |
| `delete_chunked.sh`        | DELETE with chunked transfer-encoding               |

---

## Included Test Sites

The project includes 3 test websites in `html/`:

| Directory  | Description |
|------------|-------------|
| `html/www` | Main site with docs, images, uploads, CGI scripts, error pages, CSS themes |
| `html/bob` | Simple site with custom error pages |
| `html/dos` | Comprehensive test site: autoindex, CGI, chunked, DELETE, errors, GET, POST, redirects, stress, uploads |

# Ressources

RFC:
- https://datatracker.ietf.org/doc/html/rfc2616
- https://datatracker.ietf.org/doc/html/rfc7231
- https://www.rfc-editor.org/

HTTP GUIDE:
- https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status
- https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides

Socket:
- https://www.codequoi.com/programmation-reseau-via-socket-en-c/
- https://www.youtube.com/watch?v=D26sUZ6DHNQ
- https://www.geeksforgeeks.org/c/socket-programming-cc/
- https://www.geeksforgeeks.org/cpp/socket-programming-in-cpp/

Poll:
- https://www.greenend.org.uk/rjk/tech/poll.html
- https://cboard.cprogramming.com/c-programming/158125-sockets-using-poll.html

CGI:
- https://www.garshol.priv.no/download/text/http-tut.html
- https://web.developpez.com/cgic.htm
- https://www.snv.jussieu.fr/manual/fr/howto/cgi.html
- https://www.youtube.com/watch?v=RpR_jEoAlxw

Multipart:
- https://medium.com/@muhebollah.diu/understanding-multipart-form-data-the-ultimate-guide-for-beginners-fd039c04553d
- https://stackoverflow.com/questions/16958448/what-is-http-multipart-request

Peg:
- https://bford.info/pub/lang/peg.pdf

Other:
- https://man7.org/
- https://stackoverflow.com/questions
- https://en.cppreference.com/w/cpp.html

## AI Usage

- Helping write `netcat`-based shell scripts (`script/`) for automated HTTP request testing
- Building malformed inputs (chunked encoding, oversized headers, incomplete requests)
- Clarifying RFC specifications, understanding HTTP/1.1 semantics (keep-alive, chunked transfer-encoding, status codes)
- Asking questions about unexpected behaviors (e.g., `poll()` edge cases, CGI pipe handling, multipart handling, cookies and sessions management)
- Help refacto some functions
- Building site for testing features of the server
- Iterating on the Reactor architecture (event loop invariants, connection lifecycle, non-blocking constraints) to converge toward a robust poll-driven design.
- Assisting in translating Bryan Ford’s PEG formalism into a concrete C++98 recursive-descent parser implementation.