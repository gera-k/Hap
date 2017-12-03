#include <sha2.h>
#include <stdint.h>

typedef sha512_ctx SHACTX;
#define SHAInit sha512_init
#define SHAUpdate(c, m, l) sha512_update((c), (const unsigned char *)(m), (l))
#define SHAFinal(d, c) sha512_final((c), (d))
#define SHA_DIGESTSIZE SHA512_DIGEST_SIZE
