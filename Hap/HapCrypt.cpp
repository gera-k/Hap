#include "Hap.h"

// sha2/hmac
#include "crypt/sha2.c"
#include "crypt/hmac_sha2.c"

// hkdf
#include "hkdf.c"

// chacha20
typedef unsigned int u_int;
typedef unsigned char u_char;
#include "chacha-merged.c"

// poly1305
#include "poly1305-donna.c"

// curve25519
#include "curve25519.c"

// ed25519
typedef sha512_ctx sha512_context;
#include "ed25519_fe.c"
#include "ed25519_ge.c"
#include "ed25519_keypair.c"
#include "ed25519_sc.c"
#include "ed25519_sign.c"
#include "ed25519_verify.c"

#include "Hap.h"

namespace Hap
{
	namespace Crypt
	{
		void aead(Action action,
			uint8_t* out,
			uint8_t* tag,
			const uint8_t* key,
			const uint8_t* nonce,
			const uint8_t* msg,
			uint16_t msg_size,
			const uint8_t* aad,
			uint16_t aad_size
		)
		{
			uint8_t otk[64];
			uint8_t zero[16];
			union
			{
				uint8_t b[8];
				uint64_t u;
			} num;
			size_t l;

			memset(zero, 0, sizeof(zero));

			chacha20_block(otk, key, nonce);

			chacha20_encrypt(out, msg, msg_size, key, nonce);

			poly1305_context ctx;
			poly1305_init(&ctx, otk);
			if (aad_size > 0)
			{
				l = aad_size;
				poly1305_update(&ctx, aad, l);
				if (l % 16)
					poly1305_update(&ctx, zero, 16 - (l % 16));
			}

			l = msg_size;
			if (action == Encrypt)
				poly1305_update(&ctx, out, l);
			else
				poly1305_update(&ctx, msg, l);
			if (l % 16)
				poly1305_update(&ctx, zero, 16 - (l % 16));

			num.u = aad_size;
			poly1305_update(&ctx, num.b, 8);

			num.u = msg_size;
			poly1305_update(&ctx, num.b, 8);

			poly1305_finish(&ctx, tag);
		}

		void hkdf(
			const unsigned char *salt, size_t salt_len,
			const unsigned char *key, size_t key_len,
			const unsigned char *info, size_t info_len,
			unsigned char *okm, unsigned int okm_len
		)
		{
			unsigned char prk[64];
			hmac_sha512_ctx ctx;
			uint8_t ctr = 1;

			hmac_sha512(salt, salt_len, key, key_len, prk, 64);

			hmac_sha512_init(&ctx, prk, 64);
			hmac_sha512_update(&ctx, info, info_len);
			hmac_sha512_update(&ctx, &ctr, 1);
			hmac_sha512_final(&ctx, prk, 64);

			memcpy(okm, prk, okm_len);
		}

		Curve25519::Curve25519()
		{
			const unsigned char basepoint[32] = { 9 };

			// generate private key
			t_random(_prvKey, KeySize);

			uint8_t* p = _prvKey;
			p[0] &= 248;
			p[31] &= 127;
			p[31] |= 64;

			// calculate public key
			curve25519(_pubKey, p, basepoint);
		};

		// return own public key
		const uint8_t* Curve25519::getPublicKey()
		{
			return _pubKey;
		}

		// generate shared secret from own private key and other public key
		const uint8_t* Curve25519::getSharedSecret(const uint8_t* pubKey)
		{

			curve25519(_secret, _prvKey, pubKey);

			return _secret;
		}

		Ed25519::Ed25519()
		{

		}
		
		void Ed25519::Init()		// create key pair
		{
			uint8_t seed[SeedSize];
			t_random(seed, SeedSize);

			ed25519_create_keypair(_pubKey, _prvKey, seed);
		}

		void Ed25519::Init(		// Init from stored key pair
			const uint8_t *pubKey,
			const uint8_t *prvKey
		)
		{
			memcpy(_pubKey, pubKey, PubKeySize);
			memcpy(_prvKey, prvKey, PrvKeySize);
		}


		void Ed25519::Sign(			// sign the message
			uint8_t *sign,			// signature buffer, SignSize
			const uint8_t *msg,
			uint16_t msg_len
			)
		{
			ed25519_sign(sign, msg, msg_len, _pubKey, _prvKey);
		}

		bool Ed25519::Verify(		// verify signature
			const uint8_t *sign,
			const uint8_t *msg,
			uint16_t msg_len,
			const uint8_t *pubKey
		)
		{
			int rc = ed25519_verify(sign, msg, msg_len, pubKey);

			return rc != 0;
		}

	}
}