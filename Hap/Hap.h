#ifndef _HAP_H_
#define _HAP_H_

#define sizeofarr(arr) (sizeof(arr) / sizeof((arr)[0]))

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <functional>

#define Log printf

namespace Hap
{
	struct Config		// global configuration, persistent across reboots
	{
		const char* name;	// Accessory name - used as initial Bonjour name and as
							//	Accessory Information Service name of aid=1
		const char* model;	// Model name (Bonjour and AIS)
		const char* id;		// Device ID (XX:XX:XX:XX:XX:XX, generated new on factory reset)
		uint32_t cn;		// Current configuration number, incremented on db change
		uint8_t ci;			// category identifier
		uint8_t sf;			// status flags

		uint16_t port;		// TCP port of HAP service in net byte order

		bool BCT;			// Bonjour Compatibility Test
	};

	// HAP session ID
	//	some DB characteristics and methods depend on HAP session context
	//	example - Event Notification state and pending events
	using sid_t = uint8_t;
	constexpr sid_t sid_invalid = 0xFF;
	constexpr sid_t sid_max = 7;		// up to 8 sessions, sid_t = 0..7 (5.2.3 TCP requirements)
}

#include "HapMdns.h"
#include "HapJson.h"
#include "HapTlv.h"
#include "HapDb.h"
#include "HapAppleCharacteristics.h"
#include "HapAppleServices.h"

#endif