#ifndef CONTENTTYPE_H
#define CONTENTTYPE_H

#include "../../helpers/bool/bool.h"

/*
struct ContentType {
    int (*build)(void);
    char *(*setContentType)(bool sameDomainReferer, const char *fileExtension);
    void (*destroy)(void);
};
typedef struct ContentType ContentType;
extern ContentType contentType;
*/

int buildContentTypeHT(void);
char *setContentType(bool sameDomainReferer, const char *fileExtension);
void destroyContentTypeHT(void);

#endif /* CONTENTTYPE_H */
