#include <srp/sph_sha2.h>
#include <stdint.h>

typedef sph_sha512_context SHACTX;
#define SHAInit sph_sha512_init
#define SHAUpdate sph_sha512
#define SHAFinal(b,c) sph_sha512_close(c,b)
#define SHA_DIGESTSIZE (SPH_SIZE_sha512/8)
