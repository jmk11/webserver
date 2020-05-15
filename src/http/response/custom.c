#include "custom.h"

/*
* Adds language and security headers
* Should be configurable
*/
int addMyHeaders(responseHeaders *headers)
{
    headers->ContentLanguage.value = "en";

	//securityheaders.com:
	headers->XContentTypeOptions.value = "nosniff";
	headers->XFrameOptions.value = "SAMEORIGIN";
	headers->ContentSecurityPolicy.value = "default-src 'self'; style-src 'unsafe-inline';";
	// 'unsafe-inline' is to allow inline css within html pages
	// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Security-Policy/style-src
	// Can use an inline style nonce or hash to only allow specific ones
	headers->StrictTransportSecurity.value = "max-age=63072000; includeSubDomains";
	headers->ReferrerPolicy.value = "same-origin";
	// Feature policy
	return 0;
}
