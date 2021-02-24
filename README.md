# webserver
HTTPS webserver supporting GET and HEAD requests.  
With HTTP server serving redirects to HTTPS server.  
Combines with directory listings made by htmldirs repo.  
This is a fun/learning project only. Written on and for Linux.

### Building
```bash
meson build --prefix="$installdirectory"
cd build
ninja
ninja install
```
Executables will be placed in $installdirectory.  
Website files go in $installdirectory/files.  
Configuration files must be in $installdirectory/config

### Dependencies
meson, ninja (for building)  
openssl

### Running
Instead of running as superuser, you can use capabilities to give the binaries permission to bind on ports <1000:
```bash
setcap CAP_NET_BIND_SERVICE+ep ./webserver
setcap CAP_NET_BIND_SERVICE+ep ./httpredirect
```

Run with:
```bash
./webserver $port
```
Run HTTP server with:
```bash
./http $port
```

#### Configuration
*SSL Configuration:* Relative to the directory the executables are run in, you must provide a "config/certs.txt" file with the file locations (relative to the executable) of the certificate and the private key. Certificate location on first line, private key on second line.  
*Domain name configuration*: You must provide "config/domain.txt" file which contains only the address of the HTTPS website, eg `https://localhost`.  
Examples are included in the config directory in the repository.

### Modules
Files  
Helpers: generic hashtable, strings  
HTTP: load request headers into struct, process response headers struct into string  
SSL: set up and destroy openSSL

### Features
**Request headers taken into account:**  
HTTP version  
DNT  
Connection  
If-Modified-Since  
-- Further request headers are processed into a request headers struct by http module

**Response headers used:**  
Content-Disposition  
Content-Length  
Content-Language  
Content-Type  (implemented with hash table mapping file extension to MIME type)  
Date  
Last-Modified  
Tk  
Content-Security-Policy  
X-Content-Type-Options  
X-Frame-Options  
Referrer-Policy  
Strict-Transport-Security  
 

**Directory welcome pages:**  
1\. index.html  
2\. .directory.html (can be created by [htmldirs](https://github.com/jmk11/htmldirs) repo)  
3\. 404
