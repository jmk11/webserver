#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "files.h"
#include "../wrappers/wrappers.h"
//#include "constants.h"
#include "../helpers/bool/bool.h"

// bool isReadable(const char *filepath);

/*
* Returns file size, and puts last modified time in *lastModified
* On failure or special circumstance, returns one of negative constants specifying situation
*/
/*
off_t getFileDetails(const char *filepath, time_t *lastModified) 
{
	struct stat filestat;
	int res = stat(filepath, &filestat);
	if (res != 0) { return STAT_NOTFOUND; } // perror
	//mode_t fileType = filestat.st_mode & S_IFMT;
	//if (fileType == S_IFDIR) { return STAT_ISDIR; }
	if ((filestat.st_mode & S_IFDIR) != 0) { return STAT_ISDIR; }
	if (filestat.st_size < 0) { return STAT_NOTFOUND; } // I'm not sure if this is possible..
	//if (filestat.st_size > INT_MAX) { return STAT_TOOLARGE; }
	*lastModified = filestat.st_mtime;
	if (! isReadable(filepath)) {
		return STAT_NOTREADABLE;
	}
	return filestat.st_size;
	// putting in explicit conversion to show that I have done the work to make sure it is okay
	// stat has time of last modification, for cache
}
*/

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


/*
* Loads file at filepath into *filebuf
* *filebuf is set to malloced memory that must be freed
*/
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
	if (bytesRead < 0 || (size_t) bytesRead != filesize) {
		return FILEERROR;
	}
	
	int res = close(fd);
	if (res != 0) {
		perror("Error close");
	}
	return 0;
}
