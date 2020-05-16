#ifndef FILES_H
#define FILES_H

#include <sys/types.h>

#include "../helpers/bool/bool.h"

// these must be negative!!
#define STAT_NOTFOUND -1
#define STAT_TOOLARGE -2
#define STAT_NOTREADABLE -3
#define STAT_NOTMODIFIED -4
#define STAT_ISDIR -5

//off_t getFileDetails(const char *filepath, time_t *lastModified);
int loadRequestedFile(const char *filepath, char **filebuf, size_t filesize);
bool isReadable(const char *filepath);


#endif /* FILES_H */
