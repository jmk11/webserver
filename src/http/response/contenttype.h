#ifndef CONTENTTYPE_H
#define CONTENTTYPE_H

#include "../../helpers/bool/bool.h"

struct ContentType {
    int (*build)(void);
    char *(*setContentType)(bool sameDomainReferer, const char *fileExtension);
    void (*destroy)(void);
};
typedef struct ContentType ContentType;
//contentType contentType = {.build = buildContentTypeHT}

/*
int buildContentTypeHT(void);
char *setContentType(bool sameDomainReferer, const char *fileExtension);
void destroyContentTypeHT(void);
*/

extern ContentType contentType;


#endif /* CONTENTTYPE_H */
