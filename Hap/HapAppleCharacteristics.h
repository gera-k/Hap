#ifndef _HAP_APPLE_CHARACTERISTICS_H_
#define _HAP_APPLE_CHARACTERISTICS_H_

namespace Hap
{
	namespace Characteristic
	{
		// Firmware Revision
		//	UUID 00000052-0000-1000-8000-0026BB765291
		//	Permissions		Paired Read
		//	Format			string
		class FirmwareRevision : public Simple< 0, FormatId::ConstStr>
		{
		public:
			static constexpr const char* Type = "52";

			FirmwareRevision(Simple::V value = nullptr) : Simple(
				Type,
				Hap::Property::Permissions::PairedWrite
			)
			{
				Value(value);
			}
		};

		// Identify
		//	UUID 00000014-0000-1000-8000-0026BB765291
		//	Permissions Paired Write
		//	Format		bool
		class Identify : public Simple< 0, FormatId::Bool >
		{
		public:
			static constexpr const char* Type = "14";

			Identify(Simple::V value = false) : Simple(
				Type,
				Hap::Property::Permissions::PairedWrite
			)
			{
			}

		};

		// Manufacturer
		//	UUID 00000020-0000-1000-8000-0026BB765291
		//	Permissions		Paired Read
		//	Format			string
		//	Maximum Length	64
		class Manufacturer : public Simple< 0, FormatId::ConstStr>
		{
		public:
			static constexpr const char* Type = "20";

			Manufacturer(Simple::V value = nullptr) : Simple(
				Type,
				Hap::Property::Permissions::PairedWrite
			)
			{
				Value(value);
			}
		};

		// Model
		//	UUID 00000021-0000-1000-8000-0026BB765291
		//	Permissions		Paired Read
		//	Format			string
		//	Maximum Length	64
		class Model : public Simple< 0, FormatId::ConstStr>
		{
		public:
			static constexpr const char* Type = "21";

			Model(Simple::V value = nullptr) : Simple(
				Type,
				Hap::Property::Permissions::PairedWrite
			)
			{
				Value(value);
			}
		};

		// Name
		//	UUID 00000023-0000-1000-8000-0026BB765291
		//	Permissions		Paired Read
		//	Format			string
		//	Maximum Length	64
		class Name : public Simple< 0, FormatId::ConstStr>
		{
		public:
			static constexpr const char* Type = "23";

			Name(Simple::V value = nullptr) : Simple(
				Type,
				Hap::Property::Permissions::PairedWrite
			)
			{
				Value(value);
			}
		};

		// On
		//	UUID 00000025-0000-1000-8000-0026BB765291
		//	Permissions Paired Read, Paired Write, Notify
		//	Format		bool
		class On : public Simple< 0, FormatId::Bool >
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

		// SerialNumber
		//	UUID 00000030-0000-1000-8000-0026BB765291
		//	Permissions		Paired Read
		//	Format			string
		//	Maximum Length	64
		class SerialNumber : public Simple< 0, FormatId::ConstStr>
		{
		public:
			static constexpr const char* Type = "30";

			SerialNumber(Simple::V value = nullptr) : Simple(
				Type,
				Hap::Property::Permissions::PairedWrite
			)
			{
				Value(value);
			}
		};

	}
}

#endif