#ifndef _HAP_APPLE_CHARACTERISTICS_H_
#define _HAP_APPLE_CHARACTERISTICS_H_

namespace Hap
{
	namespace Characteristic
	{
		// Hap::Characteristic::On
		//	This characteristic represents the states for "on" and "off"
		//	UUID 00000025-0000-1000-8000-0026BB765291
		//	Type public.hap.characteristic.on
		//	Permissions Paired Read, Paired Write, Notify
		//	Format bool
		class On : public Simple< 1, FormatId::Bool >
		{
		public:
			static constexpr const char* Type = "25";

			On(Simple::V value = false) : Simple(
				Type,
				Hap::Property::Permissions::PairedRead |
				Hap::Property::Permissions::PairedWrite |
				Hap::Property::Permissions::Events
			)
			{
				Value(value);
			}

			void on()
			{
				Value(true);
			}

			void off()
			{
				Value(false);
			}
		};
	}
}

#endif