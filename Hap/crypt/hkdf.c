#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "hkdf.h"

#if 0
static unsigned char *HKDF_Expand(
    const unsigned char *prk, size_t prk_len,
    const unsigned char *info, size_t info_len,
    unsigned char *okm, unsigned int okm_len
)
{
	hmac_sha512_ctx *ctx;
    unsigned int i;
    unsigned char prev[64];
    size_t done_len = 0, dig_len = 64;
    size_t n = okm_len / dig_len;

    if (okm_len % dig_len)
        n++;

    if (n > 255)
        return NULL;

    HMAC_CTX_init(&hmac);

    if (!HMAC_Init_ex(&hmac, prk, prk_len, evp_md, NULL))
	return NULL;

    for (i = 1; i <= n; i++) {
        size_t copy_len;
        const unsigned char ctr = i;

        if (i > 1) {
            if (!HMAC_Init_ex(&hmac, NULL, 0, NULL, NULL))
		return NULL;

            if (!HMAC_Update(&hmac, prev, dig_len))
		return NULL;
        }

        if (!HMAC_Update(&hmac, info, info_len))
	    return NULL;

        if (!HMAC_Update(&hmac, &ctr, 1))
	    return NULL;

        if (!HMAC_Final(&hmac, prev, NULL))
	    return NULL;

        copy_len = (done_len + dig_len > okm_len) ?
                       okm_len - done_len :
                       dig_len;

        memcpy(okm + done_len, prev, copy_len);

        done_len += copy_len;
    }

    return okm;
}
#endif


