#ifndef CONTENTTYPE_H
#define CONTENTTYPE_H

int buildContentTypeHT(void);
int setContentType(const char *fileExtension, char **contentType);
void destroyContentTypeHT(void);

#endif /* CONTENTTYPE_H */
