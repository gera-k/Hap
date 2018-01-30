/*
MIT License

Copyright (c) 2018 Gera Kazakov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _HAP_CRYPT_H_
#define _HAP_CRYPT_H_

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
			
			Curve25519() {}

			// init keys - create new key pair
			void Init();

			// return own public key
			const uint8_t* getPublicKey();

			// generate shared secret from own private key and other public key
			const uint8_t* getSharedSecret(const uint8_t* pubKey = nullptr);

		private:
			uint8_t _prvKey[KeySize];
			uint8_t _pubKey[KeySize];
			uint8_t _secret[KeySize];
		};

		class Ed25519
		{
		public:
			constexpr static uint8_t SignSize = 64;
			constexpr static uint8_t SeedSize = 32;
			constexpr static uint8_t PubKeySize = 32;
			constexpr static uint8_t PrvKeySize = 64;

			Ed25519();

			// return own public key
			const uint8_t* PubKey()
			{
				return _pubKey;
			}

			void Sign(					// sign the message
				uint8_t *sign,			// signature buffer, SignSize
				const uint8_t *msg, 
				uint16_t msg_len
			);

			bool Verify(				// verify signature
				const uint8_t *sign,	// signature, SignSize
				const uint8_t *msg,
				uint16_t msg_len,
				const uint8_t *pubKey	// other side public key
			);
		
		protected:
			void init();				// create key pair

			void init(					// Init from stored key pair
				const uint8_t *pubKey,
				const uint8_t *prvKey
			);

			uint8_t _prvKey[PrvKeySize];
			uint8_t _pubKey[PubKeySize];
		};
	}
}



#endif

