# webserver
HTTPS webserver supporting GET and HEAD requests.  
And HTTP server serving redirects to HTTPS server.  
Combines with directory listings made by htmldirs repo.
This is a fun/learning project only.

#### Building:
```bash
meson build --prefix="$installdirectory"
cd build
ninja
ninja install
```
Executables will be placed in $installdirectory.  
Website files go in $installdirectory/files.  
Configuration files must be in $installdirectory/config

#### Dependencies:
meson, ninja (for building)  
openssl

#### Running:
Run with:
```bash
./webserver $port
```
Run HTTP server with:
```bash
./http $port
```

#### Features
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
X-Frame-Options  
X-Content-Type-Options  
Content-Security-Policy  
Strict-Transport-Security  
Referrer-Policy  

**Directory welcome pages:**  
1\. index.html  
2\. .directory.html (can be created by [htmldirs](https://github.com/jmk11/htmldirs) repo)  
3\. 404

#### Modules
Files  
Helpers: generic hashtable, strings  
HTTP  
SSL
