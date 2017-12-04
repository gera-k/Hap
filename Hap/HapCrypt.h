#ifndef _HAP_CRYPTO_H_
#define _HAP_CRYPTO_H_

#include "Hap.h"

// Interface to Crypto algorithms

namespace Hap
{
	namespace Crypt
	{

		class Aead
		{
		public:
			constexpr static uint16_t TagSize = 16;
			enum Action
			{
				Encrypt = 0,
				Decrypt = 1
			};

			Aead(Action action,
				uint8_t* out,
				uint8_t* tag,
				const uint8_t* key,
				const uint8_t* nonce,
				const uint8_t* msg,
				uint16_t msg_size,
				const uint8_t* aad = nullptr,
				uint16_t aad_size = 0
			);
		};
	}
}





#endif

