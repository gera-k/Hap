#ifndef _HAP_CRYPTO_H_
#define _HAP_CRYPTO_H_

#include "Hap.h"

// Interface to Crypto algorithms

namespace Hap
{
	namespace Crypt
	{

		constexpr static uint16_t TagSize = 16;
		enum Action
		{
			Encrypt = 0,
			Decrypt = 1
		};

		void aead(Action action,
			uint8_t* out,
			uint8_t* tag,
			const uint8_t* key,
			const uint8_t* nonce,
			const uint8_t* msg,
			uint16_t msg_size,
			const uint8_t* aad = nullptr,
			uint16_t aad_size = 0
		);

		void hkdf(
			const unsigned char *salt, size_t salt_len,
			const unsigned char *key, size_t key_len,
			const unsigned char *info, size_t info_len,
			unsigned char *okm, unsigned int okm_len
		);

		class Curve25519
		{
		public:
			constexpr static uint8_t KeySize = 32;
			
			Curve25519();

			// return own public key
			const uint8_t* getPublicKey();

			// generate shared secret from own private key and other public key
			const uint8_t* getSharedSecret(const uint8_t* pubKey);

		private:
			uint8_t _prvKey[KeySize];
			uint8_t _pubKey[KeySize];
			uint8_t _secret[KeySize];
		};


	}
}



#endif

