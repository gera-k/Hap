#ifndef _HAP_H_
#define _HAP_H_

#define sizeofarr(arr) (sizeof(arr) / sizeof((arr)[0]))

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <functional>

extern "C" void t_random(unsigned char* data, unsigned size);

#define Log printf
void Hex(const char* Header, const void* Buffer, size_t Length);

namespace Hap
{
	// global constants
	constexpr uint8_t MaxPairings = 16;						// max number of pairings the accessory supports (4.11 Add pairing)
	constexpr uint8_t MaxHttpSessions = 8;					// max HTTP sessions (5.2.3 TCP requirements)
	constexpr uint8_t MaxHttpHeaders = 20;					// max number of HTTP headers in request
	constexpr uint8_t MaxHttpTlv = 10;						// max num of items in incoming TLV
	constexpr uint16_t MaxHttpFrame = 2 + 1024 + 16;		// max HTTP frame - fits max single encrypted frame (5.5.2 Session securiry)

															// global configuration, persistent across reboots

	namespace Bonjour
	{
		enum FeatureFlag
		{
			SupportsHapPairing = 1,
		};

		enum StatusFlag
		{
			NotPaired = 0x01,
			NotConfiguredForWiFi = 0x02,
			ProblemDetected = 0x04
		};
	}

	struct Config
	{
		const char* name;	// Accessory name - used as initial Bonjour name and as
							//	Accessory Information Service name of aid=1
		const char* model;	// Model name (Bonjour and AIS)
		const char* id;		// Device ID (XX:XX:XX:XX:XX:XX, generated new on factory reset)
		uint32_t cn;		// Current configuration number, incremented on db change
		uint8_t ci;			// category identifier
		uint8_t sf;			// status flags

		const char* setup;	// setup code

		uint16_t port;		// TCP port of HAP service in net byte order

		bool BCT;			// Bonjour Compatibility Test

		std::function<void()> MdnsUpdate;
	};

	extern Config config;

	// HAP session ID
	//	some DB characteristics and methods depend on HAP session context
	//	example - Event Notification state and pending events
	using sid_t = uint8_t;
	constexpr sid_t sid_invalid = 0xFF;
	constexpr sid_t sid_max = MaxHttpSessions - 1;

	// iOS device
	struct Controller
	{
		constexpr static uint8_t IdLen = 36;	// max size of controller ID
		constexpr static uint8_t KeyLen = 32;	// size of controller public key
		using Id = uint8_t[IdLen];
		using Key = uint8_t[KeyLen];

		// permissions
		enum Perm
		{
			None = 0xFF,
			Regular = 0,
			Admin = 1,
		};

		Perm perm;
		Id id;
		Key key;
	};

	// forward declarations
	class Db;
	class Pairings;
}

#include "HapSrp.h"
#include "HapCrypt.h"
#include "HapMdns.h"
#include "HapJson.h"
#include "HapTlv.h"
#include "HapHttp.h"
#include "HapTcp.h"
#include "HapDb.h"
#include "HapAppleCharacteristics.h"
#include "HapAppleServices.h"

namespace Hap
{
	// pairings DB, persistent across reboots
	class Pairings
	{
	public:
	
		void Init()		// Init pairings - destroy all existing records
		{
			for (int i = 0; i < sizeofarr(_db); i++)
			{
				Controller* ios = &_db[i];

				ios->perm = Controller::None;
			}
		}

		// count pairing records with matching Permissions
		//	in perm == None, cput all records
		uint8_t Count(Controller::Perm perm = Controller::None)
		{
			uint8_t cnt = 0;
			for (int i = 0; i < sizeofarr(_db); i++)
			{
				Controller* ios = &_db[i];
				
				if (ios->perm == Controller::None)
					continue;

				if (perm == Controller::None || perm == ios->perm)
					cnt++;
			}

			return cnt;
		}

		bool Add(const uint8_t* id, uint8_t id_len, const uint8_t* key, Controller::Perm perm);
		bool Add(const Hap::Tlv::Item& id, const Hap::Tlv::Item& key, Controller::Perm perm);

		const Controller* Get(const Hap::Tlv::Item& id);

	private:
		Controller _db[MaxPairings];
	};
}

#endif