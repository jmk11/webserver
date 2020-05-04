# webserver
HTTPS webserver supporting GET and HEAD requests.  
And HTTP server serving redirects to HTTPS server.  
Combines with directory listings made by htmldirs repo.

Build with:
```bash
$ meson build --prefix="$installdirectory"
cd build
ninja
ninja install
```
