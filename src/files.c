#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "files.h"
#include "wrappers.h"
//#include "constants.h"
#include "bool/bool.h"

bool isReadable(const char *filepath);

/*
* Returns file size
* or negative constants to communicate problem
*/
off_t getFileDetails(const char *filepath, time_t *modifiedSince) 
{
	struct stat filestat;
	int res = stat(filepath, &filestat);
	if (res != 0) { /* perror */ return STAT_NOTFOUND; }
	if (filestat.st_size < 0) { return STAT_NOTFOUND; } // I'm not sure if this is possible..
	//if (filestat.st_size > INT_MAX) { return STAT_TOOLARGE; }
	*modifiedSince = filestat.st_mtime;
	if (! isReadable(filepath)) {
		return STAT_NOTREADABLE;
	}
	return filestat.st_size;
	// putting in explicit conversion to show that I have done the work to make sure it is okay
	// stat has time of last modification, for cache
}

// is this worth the system calls of getuid and getgid when I can just fail open()
bool isReadable(const char *filepath)
{
    int fd = open(filepath, O_RDONLY);
    close(fd);
    return fd >= 0;
    /*
	// cut out from 1521 Jashank
	// must be a regular file
	if (! S_ISREG (s.st_mode)) { return FALSE; }
	// if it's owner executable by us, ok
	if (s.st_uid == getuid() && s.st_mode & S_IRUSR) return TRUE;
	// if it's group executable by us, ok
	// what about user in multiple groups?
	if (s.st_gid == getgid() && s.st_mode & S_IRGRP) return TRUE;
	// if it's other executable by us, ok
	if (s.st_mode & S_IROTH) return TRUE;
	// otherwise, no, we can't execute it.
    */
}


// sets filebuf pointer to malloced memory that must be freed
int loadRequestedFile(const char *filepath, char **filebuf, size_t filesize) 
{	
	// if file permissions suit
	
	int fd = open(filepath, O_RDONLY);
	if (fd < 0) {
		return FILEERROR;
	}
	*filebuf = malloc(filesize);
	if (*filebuf == NULL) {
		return MALLOCERROR;
	}
	ssize_t bytesRead = read(fd, *filebuf, filesize);
	if (bytesRead != filesize) {
		return FILEERROR;
	}
	
	int res = close(fd);
	if (res != 0) {
		perror("Error close");
	}
	return 0;
}
