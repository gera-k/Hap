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

			void Init();				// create key pair
			
			void Init(					// Init from stored key pair
				const uint8_t *pubKey,
				const uint8_t *prvKey
			);

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
		
		private:
			uint8_t _prvKey[PrvKeySize];
			uint8_t _pubKey[PubKeySize];
		};
	}
}



#endif

