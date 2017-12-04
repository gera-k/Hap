#ifndef ED25519_H
#define ED25519_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ED25519_SIGN_SIZE 64
#define ED25519_PKEY_SIZE 32
#define ED25519_SKEY_SIZE 64
#define ED25519_SEED_SIZE 32

void ed25519_create_keypair(unsigned char *public_key, unsigned char *private_key, const unsigned char *seed);
void ed25519_sign(unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key, const unsigned char *private_key);
int ed25519_verify(const unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key);


#ifdef __cplusplus
}
#endif

#endif
