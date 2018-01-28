#ifndef _HAP_APPLE_CHARACTERISTICS_H_
#define _HAP_APPLE_CHARACTERISTICS_H_

namespace Hap
{
	namespace Characteristic
	{
		// Brightness
		//	UUID 00000008-0000-1000-8000-0026BB765291
		//	Permissions 	Paired Read, Paired Write, Notify
		//	Format 			int
		//	Minimum Value	0
		//	Maximum Value	100
		//	Step Value		1
		//	Unit 			percentage
		class Brightness : public Simple< 4, FormatId::Int>
		{
		protected:
			Property::MinValue<FormatId::Int> _min;
			Property::MaxValue<FormatId::Int> _max;
			Property::MinStep<FormatId::Int> _step;
			Property::Unit _unit;
		public:
			static constexpr const char* Type = "8";

			Brightness(Simple::V value = 0) : Simple(
				Type,
				Property::Permissions::PairedRead |
				Property::Permissions::PairedWrite |
				Property::Permissions::Events
			)
			{
				AddProperty(&_min);
				AddProperty(&_max);
				AddProperty(&_step);
				AddProperty(&_unit);

				_min.set(0);
				_max.set(100);
				_step.set(1);
				_unit.set(Property::Unit::T::percentage);

				Value(value);
			}

		};

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
				Property::Permissions::PairedRead
			)
			{
				Value(value);
			}
		};

		// Hardware Revision
		//	UUID 00000053-0000-1000-8000-0026BB765291
		//	Permissions		Paired Read
		//	Format			string
		class HardwareRevision : public Simple< 0, FormatId::ConstStr>
		{
		public:
			static constexpr const char* Type = "52";

			HardwareRevision(Simple::V value = nullptr) : Simple(
				Type,
				Property::Permissions::PairedRead
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
				Property::Permissions::PairedWrite
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
				Property::Permissions::PairedRead
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
				Property::Permissions::PairedRead
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
				Property::Permissions::PairedRead
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
				Property::Permissions::PairedRead |
				Property::Permissions::PairedWrite |
				Property::Permissions::Events
			)
			{
				Value(value);
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
				Property::Permissions::PairedRead
			)
			{
				Value(value);
			}
		};

	}
}

#endif
