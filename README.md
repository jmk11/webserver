# webserver
HTTPS webserver supporting GET and HEAD requests.  
And HTTP server serving redirects to HTTPS server.  
Combines with directory listings made by htmldirs repo.

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
Request headers taken into account:  
DNT  
Connection  
If-Modified-Since  

Response headers used:  
Content-Length  
Content-Language  
Content-Type  
Date  
Last-Modified  
Tk  
X-Frame-Options  
X-Content-Type-Options  
Content-Security-Policy  
Strict-Transport-Security  
Referrer-Policy  
