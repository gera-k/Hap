#include "HapCrypt.h"

// chacha20
typedef unsigned int u_int;
typedef unsigned char u_char;
#include "chacha-merged.c"

// poly1305
#include "poly1305-donna.c"


#include "Hap.h"

namespace Hap
{
	namespace Crypt
	{
		Aead::Aead(Action action,
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

			//_data.resize(msg.size());

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

	}
}