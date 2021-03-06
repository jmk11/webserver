// #include <stdio.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <sys/types.h>
// #include <stdlib.h>
// #include <assert.h>
#include <sys/stat.h>
// #include <limits.h>
// #include <linux/limits.h>
// #include <string.h>
#include <string.h>
#include <openssl/ssl.h>
#include <fcntl.h>
#include <unistd.h>

#include "connection.h"
#include "logging.h"
#include "constants.h"
#include "wrappers/wrappers.h" // only needs this for error codes, this isn't right
#include "files/files.h"
#include "http/statuscodes.h"
#include "http/response/response.h"
#include "helpers/strings/strings.h"
// #include "helpers/bool/bool.h"
// #include "http/helpers.h"


#define FILESDIRECTORY "files/"
#define FILECUTOFF 10000000
#define INMEM404 "<html><head><title>404</title></head><body>404</body></html>"

int handleRequest(SSL *ssl, char *requestbuf, int logfd, struct sockaddr_in source, const char *domain);
int sanitiseResource(const char *resource);
char *getExtension(const char *filepath);
char *nullbyte(void);
int buildFilePath(const char *filename, char *filepath, unsigned int filepathsize);
int fileNotFound(SSL *ssl, responseHeaders *headers, const requestHeaders *requestHeaders, const char *statusCode);
//int fileNotAvailable(SSL *ssl);
int sendFile(SSL *ssl, const char *statusCode, responseHeaders *response, const requestHeaders *request, const char *filepath, int filesize, time_t lastModified, const char *fileextension);
off_t getFileDetails(char filepath[MAXPATH], time_t *lastModified);
int redirect(SSL *ssl, const char *domain, const char *filepath1, const char *filepath2) ;

//static const size_t inmem404length = strlen(INMEM404);
static const size_t inmem404length = sizeof(INMEM404) -1; // !! check this: -1 because of null byte?

int handleConnection(int clientfd, SSL_CTX *ctx, int logfd, struct sockaddr_in source, const char *domain) {
	// set up SSL connection - from openSSL wiki
	SSL *ssl = SSL_new(ctx);
	if (ssl == NULL) { 
		return 1;
	}
	if (SSL_set_fd(ssl, clientfd) == 0) {
		return 1;
	}
	if (SSL_accept(ssl) <= 0) {
		ERR_print_errors_fp(stderr);
		return 1;
	}

	// handle requests until connection close
	char requestbuf[BUFSIZ];
	ssize_t requestLen;
	bool continueConnection = FALSE;
	do {
		// TODO: set timeout on read
		requestLen = SSL_read(ssl, requestbuf, BUFSIZ-1);
		if (requestLen <= 0) {
			fprintf(stderr, "SSL_read error\n");
			continueConnection = FALSE;
		}
		else {
			requestbuf[requestLen] = 0;
			printf("%s\n", requestbuf);
			int res = handleRequest(ssl, requestbuf, logfd, source, domain);
			continueConnection = (res >= 0);
		}
	} while (continueConnection);

	// close SSL connection
	SSL_shutdown(ssl); // surely this doesn't need to be called if accept() fails?
	SSL_free(ssl);
	return 0;
}

/*
* Handle specific request
* Return 0 on okay and continue connection
* Return 1 on bad and continue connection
* Return -1 on finish connection
*/
int handleRequest(SSL *ssl, char *requestbuf, int logfd, struct sockaddr_in source, const char *domain)//, ssize_t requestLength)
{
	printf("Response:\n");
	//int badReturn = 1;

	int filesize;
	int res;
	//char resourceArr[MAXPATH];
	char filepath[MAXPATH];
	//char *resource;
	//char fileextension[MAXPATH];

	struct responseHeaders response;
	initialiseResponseHeaders(&response);
	//response = responseHeadersBase;
	struct requestHeaders request;

	// parse headers
	const char *statusCode = parseHeaders(&request, requestbuf);
	if (statusCode != NULL) {
		// illegal headers, respond with status code
		sendResponseNoBody(ssl, &response, statusCode, &request);
		res = 1;
		goto RETURN;
		//return request.ConnectionKeep ? 1 : -1;
	}
	// do not change requestbuf from this point on
	// check that requested method fits allowed. atm this is done in parseHeaders()
	/*if (requestHeaders.method != METHOD_GET && requestHeaders.method != METHOD_HEAD) {
		// send method not supported or method not allowed
		setGenericHeaders(&headers, &requestheaders);
		sendResponse(ssl, STATUS_METHODNOTALLOWED, &headers, &requestHeaders, NULL, 0);
		return requestHeaders.ConnectionKeep ? 1 : -1;
	}*/
	logRequest(logfd, source, &request);
	
	// Convert requested resource to actual resource name
	//resource = getResource(request.resource, resourceArr);
	res = sanitiseResource(request.resource);
	if (res != 0) {
		fileNotFound(ssl, &response, &request, STATUS_NOTFOUND);
		goto RETURN;
		//return request.ConnectionKeep ? 1 : -1;
	}
	printf("Filename: %s\n", request.resource);

	// Convert resource name to file path
	res = buildFilePath(request.resource, filepath, MAXPATH);
	if (res != 0) {
		fileNotFound(ssl, &response, &request, STATUS_URITOOLONG);
		res = 1;
		goto RETURN;
		//return request.ConnectionKeep ? 1 : -1;
	}

	// not sure if this file should need to know that last modified and file size are required headers?
	// would be nice if could just give filename and let response send it
	// but would have to check that file actually exists before doing that
	// and it would be a waste to do that without getting the details

	// Check file exists and is accessible, check file type, and get file size and last modified time
	// and append to filepath index.html/.directory.html if file is a directory
	time_t lastModified;
	off_t filesizeres = getFileDetails(filepath, &lastModified);
	if (filesizeres < 0) {
		if (filesizeres == -1) {
			res = fileNotFound(ssl, &response, &request, STATUS_NOTFOUND);
		}
		else if (filesizeres == -2) {
			/*res = strlcat1(filepath, "/", MAXPATH);
			if (res != 0) {
				res = fileNotFound(ssl, &response, &request, STATUS_URITOOLONG);
			}
			else {*/
			res = redirect(ssl, domain, request.resource, "/");
			if (res != 0) {
				res = fileNotFound(ssl, &response, &request, STATUS_URITOOLONG);
			}
			else {
				res = -1;
				// clean this up
			}
		}
		goto RETURN;
	}
	else if (filesizeres > INT_MAX) {
		res = fileNotFound(ssl, &response, &request, STATUS_NOTFOUND);
		goto RETURN;
		//return request.ConnectionKeep ? res : -1;
	}
	filesize = (int) filesizeres;

	// check for 304 not modified
	if (lastModified <= request.IfModifiedSince) {
		res = sendResponseNoBody(ssl, &response, STATUS_NOTMODIFIED, &request);
		goto RETURN;
		//return request.ConnectionKeep ? res : -1;
	}

	// get file extension for content type
	char *fileextension = getExtension(filepath);
	
	// send normal response
	res = sendFile(ssl, STATUS_OK, &response, &request, filepath, filesize, lastModified, fileextension);
	free(fileextension);
	if (res != 0) {
		res = fileNotFound(ssl, &response, &request, STATUS_NOTFOUND);
		goto RETURN; // hehe
		//return request.ConnectionKeep ? res : -1;
	}
 
	RETURN:
	freeRequestHeaders(&request);
	return request.ConnectionKeep ? res : -1;
}

/*
* Get filesize and last modification time of file in filepath
* If filepath is a directory, will change filepath to the actual file to present
* On success, return filesize and fill lastModified
* On failure (file doesn't exist, permissions not suitable, ...), return -1
* On failure (file is a directory and must be redirected to filepath+'/'), return -2
* This is bad
*/
off_t getFileDetails(char filepath[MAXPATH], time_t *lastModified) 
{
	// 	if not directory:
	// 		try file
	//		if fail:
	//			fileNotFound
	// 	if directory:
	// 		try index.html
	//		if fail:
	//			try .directory.html	
	//			if fail:
	//				file NotFound

	struct stat filestat;
	int res = stat(filepath, &filestat);
	if (res != 0) {
		// perror 
		return -1;
	}
	else if ((filestat.st_mode & S_IFDIR) != 0) { // file is a directory
		if (filepath[strlen(filepath)-1] != '/') {
			return -2;
		}
		//------------------------------------------------------------
		/*
		char newfilepath[MAXPATH];
		char *options[2] = {"/index.html", "/.directory.html"};
		res = 1;
		for (unsigned int i = 0; i < 2 && res != 0; i++) {
			char *strs[3] = {filepath, options[i], NULL};
			strlcat4(newfilepath, NULL, strs, MAXPATH);
			res = stat(filepath, &filestat);
		}
		if (res != 0) {
			return -1;
		}
		strlcpy(filepath, newfilepath, MAXPATH);
		*/
		//--------------------------------------------------------------
		/*
		char newfilepath[MAXPATH];
		char *strs[3] = {filepath, "/index.html", NULL};
		strlcat4(newfilepath, NULL, strs, MAXPATH);
		res = stat(filepath, &filestat);
		if (res != 0) {
			strs[1] = "/.directory.html";
			strlcat4(newfilepath, NULL, strs, MAXPATH);
			res = stat(filepath, &filestat);
			if (res != 0) {
				return -1;
			}
		}
		strlcpy(filepath, newfilepath, MAXPATH);
		*/
		//-------------------------------------------------------------
		char filepathcopy[MAXPATH];
		strlcpy(filepathcopy, filepath, MAXPATH);
		// if res == 1

		// try index.html
		res = strlcat1(filepath, "index.html", MAXPATH);
		if (res != 0) {
			return -1;
		}
		res = stat(filepath, &filestat);
		if (res != 0) {
			// index.html doesn't exist, try .directory.html
			res = strlcat1(filepathcopy, ".directory.html", MAXPATH);
			if (res != 0) {
				return -1;
			}
			res = stat(filepathcopy, &filestat);
			if (res != 0) {
				return -1;
			}
			strlcpy(filepath, filepathcopy, MAXPATH);
			// if res == 0, .directory.html is the file and details are in filestat
		}
		//----------------------------------------------------------------
	}

	if (filestat.st_size < 0) { return -1; } // I'm not sure if this is possible..
	//if (filestat.st_size > INT_MAX) { return STAT_TOOLARGE; }
	*lastModified = filestat.st_mtime;
	if (! isReadable(filepath)) {
		return -1;
	}
	return filestat.st_size;
	// putting in explicit conversion to show that I have done the work to make sure it is okay
}

/*
*
*/
// infinite recursion if index.html or .directory.html are directories
/*
off_t getFileDetails(char filepath[MAXPATH], time_t *lastModified, char **backups) 
{
	struct stat filestat;
	int res = stat(filepath, &filestat);
	if (res != 0) {
		// perror 
		if (backups[1] != NULL) {
			res = strlcat1(filepath, backups[1], MAXPATH);
			if (res != 0) {
				return 1;
			}
			return getFileDetails(filepath, lastModified);
		}
		else if ((filestat.st_mode & S_IFDIR) != 0) { // file is a directory
			
		}
	}		
	return 0;
}
*/

// too many parameters

/*
* Send file as body of HTTP response
* Return 0 on success, 1 on failure
*/
int sendFile(SSL *ssl, const char *statusCode, responseHeaders *response, const requestHeaders *request, const char *filepath, int filesize, time_t lastModified, const char *fileextension) {
	int res;
	if (filesize <= FILECUTOFF) {
		// load file into buffer and send
		char *filebuf;
		res = loadRequestedFile(filepath, &filebuf, filesize);
		if (res != 0) {
			return 1;
		}
		res = sendResponse(ssl, statusCode, response, request, filebuf, filesize, lastModified, fileextension);
		free(filebuf);
	}
	else {
		// send buffered
		int fd = open(filepath, O_RDONLY);
		if (fd < 0) {
			return 1;
		}
		res = sendResponseBuffered(ssl, statusCode, response, request, fd, filesize, lastModified, fileextension);
		res = close(fd);
		if (res != 0) {
			perror("Error close");
		}
	}
	return 0;
}

/*
* Sanitise resource and convert resource to actual location on computer using some hardcoded conversions
* When called, resource length < bufsiz
* Returns resource on success and resource can be used as the resource
* Returns newResource if newResource should be used as the resource
* Returns NULL if resource requested was rejected during sanitisation
* // could I avoid copying except when necessary?
* // I could return a value to indicate that the resource is unchanged/changed
* // and only write to newResource if changed
*/
int sanitiseResource(const char *resource) 
{
	for (unsigned int i = 0; resource[i+1] != 0; i++) {
		if (resource[i] == '.' && resource[i+1] == '.') {
			return 1;
		}
	}
	return 0;
	// I don't like this way
	/*
	if (filename[0] == 0) {
		strncpy(newResource, "index.html", MAXPATH); // check this isn't 1 byte overflow
		return newResource;
	}*/
	// convert % encoding: // TODO: MOVE THIS TO REQUESTHEADERS.C
	// https://github.com/dnmfarrell/URI-Encode-C/blob/master/src/uri_encode.c
	// convertPCencoding(filename, newResource);
	
	// return resource;
}

// this is way too complicated and lengthy
/*
char *convertPCencoding(const char *filename, char newResource[MAXPATH]) {
	unsigned int i = 0;
	//char *filenamecur = filename;
	//bool pc = FALSE;
	// find if there is a percent
	while (filename[i] != 0 && filename[i] != '%') {
		//filenamecur++;
		i++;
	}
	if (filename[i] != '%') {
		// no percent, keep normal filename and avoid copying
		return filename;
	}

	// there is a percent
	// copy everthing up to here over to newResource
	if (i >= MAXPATH) {
		// no: the limit could be lower because we are going to lose at least one character from the percent conversion
		return NULL;
	}
	for (unsigned int j = 0; j < i; j++) {
		newResource[j] = filename[j];
	}

	char hexnum[3];
	hexnum[0] = filename[i++];
	hexnum[1] = filename[i++];
	hexnum[2] = 0;
	long long num = strtoll(hexnum, NULL, 16);
	// awful


	// now convert all percents from here to end
	for (unsigned int j = i; filename[j] != 0; j++) {

	}
}
*/

// probably worth moving this to some other place where stat() is already called
/*
* Return file extension by extracting from filepath if filepath is a regular file,
* or by resolving symbolic link and extracting from filename of final destination.
* Returned string is malloced and must be freed.
*/
char *getExtension(const char *filepath) {
	// resolve symbolic link, if it is one
	char targetpath[4096];
	filepath = realpath(filepath, targetpath);
	if (filepath == NULL) {
		perror("realpath failed");
		return nullbyte(); // maybe just return NULL and fail response
	}

	// find last dot in name
	const char *lastdot = NULL;
	for (; *filepath != 0; filepath++) {
		if (*filepath == '.') {
			lastdot = filepath;
		}
	}
	// actually, it might be faster to go all the way to end doing one comparison - just the null byte
	// and then move backwards checking for just the dot
	// might be less comparisons overall, since most of the time the . should be very near the end
	if (lastdot == NULL) { 
		return nullbyte(); 
	}
	return strdup(lastdot+1);
}

/*
* Malloc a single byte, set value to null byte, return pointer
*/
char *nullbyte() {
	char *byte = malloc(1);
	byte[0] = 0;
	return byte;
}

/*
* Build path to actual file on system
* Basically just put FILESDIRECTORY/filename into filepath
* filepath = FILESDIRECTORY + "/" + filename
*/
int buildFilePath(const char *filename, char *filepath, unsigned int filepathsize)
{
	char *prefix = FILESDIRECTORY;
	/*char *filepath = malloc(strlen(filename) + strlen(prefix) + 1);
	if (filepath == NULL) {
		perror("Can't malloc filepath");
		return NULL;
	}*/
	const char *strs[3] = {prefix, filename, NULL};
	filepath[0] = 0;
	int res = strlcat4(filepath, NULL, strs, filepathsize);
	if (res != 0) {
		return 1;
	}
	/*strcpy(filepath, prefix);
	strcat(filepath, filename);*/
	printf("Filepath: %s\n", filepath);
	return 0;
}

/*
* Build and send 404 response
* atm: returns 0 on success and 1 on failure
*/
int fileNotFound(SSL *ssl, responseHeaders *headers, const requestHeaders *requestHeaders, const char *statusCode) 
{
	// I can't call sendFile() (which was the reason I made that function), because if 404.html doesn't exist
	// it will call this and get an infinite loop
	// so I changed sendFile
	int res;
	char filepath[MAXPATH];

	res = buildFilePath("404.html", filepath, MAXPATH);
	if (res != 0) { return 1; }
	time_t lastModified;
	off_t filesizePure = getFileDetails(filepath, &lastModified);
	if (filesizePure < 0 || filesizePure > INT_MAX) {
		fprintf(stderr, "Could not access 404.html to send, or it's too big\n");
		res = sendResponse(ssl, statusCode, headers, requestHeaders, INMEM404, /*strlen(INMEM404)*/inmem404length, 0, "html");
		// stlren should be optimised out right?
	}
	else {
		int filesize = (int) filesizePure;
		res = sendFile(ssl, STATUS_NOTFOUND, headers, requestHeaders, filepath, filesize, 0, "html");
		if (res < 0) {
			res = sendResponse(ssl, statusCode, headers, requestHeaders, INMEM404, /*strlen(INMEM404)*/inmem404length, 0, "html");
		}
	}
	return res;
}

/*
* Send 301 redirect to domain/filepath1filepath2
*/
int redirect(SSL *ssl, const char *domain, const char *filepath1, const char *filepath2) 
{
	const unsigned int responsesize = 200;
	char response[responsesize];
	response[0] = 0;
	//snprintf(response, responsesize, "HTTP/1.1 301 Moved Permanently\r\nLocation: %s%s%s\r\n\r\n", domain, filepath1, filepath2);
	const char *strs[] = {"HTTP/1.1 301 Moved Permanently\r\nLocation: ", domain, "/", filepath1, filepath2, " \r\n\r\n", NULL};
	int res = strlcat4(response, NULL, strs, responsesize);
	if (res != 0) {
		return 1;
	}
	res = SSL_write(ssl, response, strlen(response));
	printf("%d %s", res, response);
	return 0;
}

/*
int fileNotAvailable(SSL *ssl) 
{
	return 0;
}
*/

/*
 * If filepath is a symbolic link, puts final destination in targetpath and returns targetpath
 * If not, returns filepath
 * On failure, returns NULL
*/
/*
char *resolveSymlink(const char *filepath, char targetpath[PATH_MAX]) {
	struct stat filestat;
	int res = lstat(filepath, &filestat);
	if (res != 0) {
		perror("lstat on filename failed");
		return NULL;
	}
	
	mode_t fileType = filestat.st_mode & S_IFMT;
	if (fileType == S_IFLNK) {
		// symbolic link
		char *reschar = realpath(filepath, targetpath);
		if (reschar == NULL) {
			perror("realpath failed");
			return NULL;
		}
		else {
			filepath = targetpath;
		}
	}
	return filepath;
}
*/

// Resolve symbolic link if file is symbolic link
	/*char *resolvedpath = resolveSymlink(filepath);
	if (resolvedPath == NULL) {
		fileNotFound(ssl, &headers, &requestheaders, STATUS_NOTFOUND);
	}*/
	// This doesn't need to be done for any file function, the OS handles it
	// it needs to be done to get the extension for the mime type

// getResource()
/*if (res != 0) {
		if (res == 1) {
			fileNotFound(ssl, &response, &request, STATUS_URITOOLONG);
		}
		else if (res == 2) {
			fileNotFound(ssl, &response, &request, STATUS_NOTFOUND);
		}
		return request.ConnectionKeep ? 1 : -1;
	}*/

/*
time_t lastModified;
	off_t filesizePure;
	GETDETAILS: 
	filesizePure = getFileDetails(filepath, &lastModified);
	if (filesizePure < 0) {
		switch (filesizePure) {
			case STAT_ISDIR:
				res = strlcat1(filepath, "/.directory.html", MAXPATH);
				if (res != 0) {
					fileNotFound(ssl, &response, &request, STATUS_URITOOLONG);
					break;
				}
				goto GETDETAILS;
				// obviously I will remove this goto
				// or maybe not, I like it now.....
				//break;
			case STAT_NOTREADABLE:
				fileNotFound(ssl, &response, &request, STATUS_NOTFOUND);
				break;
			case STAT_NOTFOUND:
				fileNotFound(ssl, &response, &request, STATUS_NOTFOUND);
				break;
			// default:
			// 	fileNotFound(ssl);
			// 	return badReturn;
		}
		res = 1;
		goto RETURN;
		//return request.ConnectionKeep ? 1 : -1;
	}
	else if (filesizePure > INT_MAX) {
		res = fileNotFound(ssl, &response, &request, STATUS_NOTFOUND);
		goto RETURN;
		//return request.ConnectionKeep ? res : -1;
	}
	filesize = (int) filesizePure
*/
