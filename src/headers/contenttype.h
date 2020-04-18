#ifndef CONTENTTYPE_H
#define CONTENTTYPE_H

#include "bool/bool.h"

int buildContentTypeHT(void);
int setContentType(bool sameDomainReferer, const char *fileExtension, char **contentType);
void destroyContentTypeHT(void);

#endif /* CONTENTTYPE_H */
