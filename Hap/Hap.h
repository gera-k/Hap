#ifndef _HAP_H_
#define _HAP_H_

#define sizeofarr(arr) (sizeof(arr) / sizeof((arr)[0]))

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <utility>
#include <functional>

extern "C" void t_random(unsigned char* data, unsigned size);

namespace Hap
{
	// Logging functions
	void Log(const char* f, ...);
	void Hex(const char* Header, const void* Buffer, size_t Length);
#define Dbg Log

	// global constants
	constexpr uint8_t MaxPairings = 10 /*16*/;				// max number of pairings the accessory supports (4.11 Add pairing)
															//	10 for now, until > 1024 byte frames are supported	TODO: fix
	constexpr uint8_t MaxHttpSessions = 8;					// max HTTP sessions (5.2.3 TCP requirements)
	constexpr uint8_t MaxHttpHeaders = 20;					// max number of HTTP headers in request
	constexpr uint8_t MaxHttpTlv = 10;						// max num of items in incoming TLV
	constexpr uint16_t MaxHttpBlock = 1024;					// max size of encrypted block (5.5.2 Session securiry)
	constexpr uint16_t MaxHttpFrame = MaxHttpBlock + 2 + 16;// max HTTP frame 

	constexpr uint16_t DefString = 64;		// default length of a string characteristic
	constexpr uint16_t MaxString = 64;		// max string length

	// generic buffer (pointer/length pair)
	template<typename T>
	class Buf : protected std::pair<T, size_t>
	{
	public:
		using B = std::pair<T, size_t>;
		Buf() {}
		Buf(T p, size_t l) : std::pair<T, size_t>(p, l) {}
		auto ptr() const { return B::first; }
		auto val() const { return B::first; }
		auto len() const { return B::second; }
		auto& ptr() { return B::first; }
		auto& len() { return B::second; }

		T& operator[](int i) { return B::first[i]; }
	};

	template<typename T, size_t S>
	class BufStatic : public Buf<T*>
	{
	private:
		T _buf[S];
	public:
		BufStatic() : Buf<T*>(_buf, S) {}
	};

	template<typename T>
	static inline Buf<T> makeBuf(T p, size_t l) { return Buf<T>(p, l); }

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

	// HAP Server configuration
	//	implementation should define methods for saving/restoring/resetting the config
	//	implementatin may save/restore all parameters, or just some of them such as config number and deviceId
	class Config
	{
	public:
		const char* name;				// Accessory name - used as initial Bonjour name and as AIS name of aid=1
		const char* model;				// Model name (Bonjour and AIS)
		const char* manufacturer;		// Manufacturer- used by AIS (Accessory Information Service)
		const char* serialNumber;		// Serial number in arbitrary format
		const char* firmwareRevision;	// Major[.Minor[.Revision]]
		const char* deviceId;			// Device ID (XX:XX:XX:XX:XX:XX, new deviceId generated on each factory reset)
		uint32_t configNum;				// Current configuration number, incremented on db change
		uint8_t categoryId;				// category identifier
		uint8_t statusFlags;			// status flags

		const char* setupCode;			// setupCode code XXX-XX-XXX
		uint16_t port;					// TCP port of HAP service in net byte order
		bool BCT;						// Bonjour Compatibility Test

		std::function<void()> Update;	// config update notification

		void Init(bool reset_ = false)
		{
			if (!_restore())	// restore saved config
				_default();	//	or create new default one	
			if (reset_)		// if reset is requested, reset some values
				_reset();
			_save();			// save new config
		}

		void Save()
		{
			_save();
		}

	protected:
		enum
		{
			key_name, 
			key_model, 
			key_manuf, 
			key_serial, 
			key_firmware, 
			key_device, 
			key_config,
			key_category, 
			key_status, 
			key_setup, 
			key_port, 
			key_keys, 
			key_pairings,
			key_db,
			key_max
		};
		static const char* key[key_max];

		virtual void _default() = 0;
		virtual void _reset() = 0;
		virtual bool _restore() = 0;
		virtual bool _save() = 0;
	};

	extern Config* config;

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
		uint8_t idLen;
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
		// count pairing records with matching Permissions
		//	in perm == None, cput all records
		uint8_t Count(Controller::Perm perm = Controller::None);

		// add pairing record, returns false if failed
		bool Add(const uint8_t* id, size_t id_len, const uint8_t* key, Controller::Perm perm);
		bool Add(const Hap::Tlv::Item& id, const Hap::Tlv::Item& key, Controller::Perm perm);

		// update controller permissions
		bool Update(const Hap::Tlv::Item& id, Controller::Perm perm);

		// remove controller
		bool Remove(const Hap::Tlv::Item& id);

		// get pairing record, returns nullptr if not found
		const Controller* Get(const Hap::Tlv::Item& id);

		bool forEach(std::function<bool(const Controller*)> cb);

	protected:
		// Init pairings - destroy all existing records
		void init();

		Controller _db[MaxPairings];
	};
}

static inline uint16_t swap_16(uint16_t v)
{
	return ((v & 0xFF) << 8) | (v >> 8);
}

#endif
