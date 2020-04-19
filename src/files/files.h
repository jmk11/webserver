#ifndef FILES_H
#define FILES_H

#include <sys/types.h>

// these must be negative!!
#define STAT_NOTFOUND -1
#define STAT_TOOLARGE -2
#define STAT_NOTREADABLE -3
#define STAT_NOTMODIFIED -4

off_t getFileDetails(const char *filepath, time_t *modifiedSince);
int loadRequestedFile(const char *filepath, char **filebuf, size_t filesize);

#endif /* FILES_H */
