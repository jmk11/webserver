# webserver
HTTPS webserver supporting GET and HEAD requests.  
And HTTP server serving redirects to HTTPS server.  
Combines with directory listings made by htmldirs repo.

Build with:
```bash
meson build --prefix="$installdirectory"
cd build
ninja
ninja install
```

Executables will be placed in $installdirectory.  
Website files go in $installdirectory/files
Configuration file must be in $installdirectory/config

Run with:
```bash
./webserver $port
```
Run HTTP server with:
```bash
./http $port
```
