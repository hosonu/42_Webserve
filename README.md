# 42_Webserve

A lightweight HTTP/1.1 compliant web server implementation

## Overview

This project is a simple and efficient web server implemented in C++.
It uses asynchronous I/O processing and supports configurable static file serving and CGI execution.

## Features

- Non-blocking I/O (using epoll)
- HTTP methods (GET, POST, DELETE)
- Server configuration via config files
- Static file serving
- CGI support
- File upload capability
- Custom error pages
- Multi-port listening

## Configuration

The server is configured using configuration files in the `config/` directory. Example configuration:

```nginx
server {
    listen localhost:8080;
    error_page 404 /error.html;
    client_max_body_size 1m;
    
    location / {
        autoindex on;
        allow_methods GET POST;
        isCGI on;
    }
}
```

### Configuration Options

- `listen`: Specify host and port (e.g., `localhost:8080`)
- `error_page`: Custom error page paths
- `client_max_body_size`: Maximum allowed request body size
- `location`: Path-specific configurations
  - `autoindex`: Enable/disable directory listing
  - `allow_methods`: Allowed HTTP methods
  - `isCGI`: Enable CGI processing
  - `root`: Document root directory
  - `index`: Default index file
  - `return`: URL redirection

## Requirements

- C++98

## Build Instructions

```bash
make
```