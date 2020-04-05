#include "custom.h"

int addMyHeaders(responseHeaders *headers)
{
    headers->ContentLanguage.value = "en";

	//securityheaders.com:
	headers->XContentTypeOptions.value = "nosniff";
	headers->XFrameOptions.value = "SAMEORIGIN";
	headers->ContentSecurityPolicy.value = "default-src 'self'";
	return 0;
}