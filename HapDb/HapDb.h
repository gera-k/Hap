#ifndef _HAP_DB_H_
#define _HAP_DB_H_

namespace Hap
{
	namespace Property
	{
		// 'unit' enumerator values and string representations
		enum class UnitId : uint8_t
		{
			Celsius,
			Percentage,
			Arcdegrees,
			Lux,
			Seconds
		};
		const char* const UnitStr[] =
		{
			"celsius",
			"percentage",
			"arcdegrees",
			"lux",
			"seconds"
		};

		// Property 'key' enum and string representation
		enum class KeyId : uint8_t
		{
			AccessoryInstanceId,
			Services,

			Type,
			InstanceId,
			Characteristics,
			HiddenServices,
			PrimaryServices,
			LinkedServices,

			Value,
			Permissions,
			EventNotifications,
			Description,
			Format,
			Unit,
			MinimumValue,
			MaximumValue,
			StepValue,
			MaxLength,
			MaxDataLength,
			ValidValues,
			ValidValuesRange
		};
		const char* const KeyStr[] =
		{
			"aid",
			"services",

			"type",
			"iid",
			"characteristics",
			"hidden",
			"primary",
			"linked",

			"value",
			"perms",
			"ev",
			"description",
			"format",
			"unit",
			"minValue",
			"maxValue",
			"minStep",
			"maxLen",
			"maxDataLen",
			"valid-values",
			"valid-values-range"
		};

		// Property 'format' enum and string representation
		//	also used to define format of all other properties
		enum class FormatId : uint8_t
		{
			Null = 0,

			// base properties
			Bool,			// bool
			Uint8,			// uint8_t
			Uint16,			// uint16_t
			Uint32,			// uint32_t
			Uint64,			// uint64_t
			Int,			// int32_t
			Float,			// double
			ConstStr,		// const char *

			// enumerated properties (internal representation - uint8_t, external - string)
			Format,			// FormatId
			Unit,			// UnitId	

			// variable-size properties	- max size is set on compile time
			String,			// char[S]
			Tlv8,			// uint8_t[S]
			Data,			// uint8_t[S]

			IdArray,		// uint64_t[S]
			PtrArray,		// void*[S]
		};
		const char* const FormatStr[] =
		{
			"null",
			"bool",
			"uint8",
			"uint16",
			"uint32",
			"uint64",
			"int",
			"float",
			"string",
			nullptr,
			nullptr,
			"string",
			"tlv8",
			"data",
			nullptr,
			nullptr
		};

		// this set of templates maps FormatId -> C type and format function
		template <FormatId> struct _hap_type;
		template<> struct _hap_type<FormatId::Null>
		{
			using type = uint8_t;
			static int getDb(char* s, size_t max, type v)
			{
				return snprintf(s, max, "null");
			}
		};
		template<> struct _hap_type<FormatId::Bool>
		{
			using type = bool;
			static int getDb(char* s, size_t max, type v)
			{
				return snprintf(s, max, "%s", v ? "true" : "false");
			}
		};
		template<> struct _hap_type<FormatId::Uint8>
		{
			using type = uint8_t;
			static int getDb(char* s, size_t max, type v)
			{
				return snprintf(s, max, "%u", v);
			}
		};
		template<> struct _hap_type<FormatId::Uint16>
		{
			using type = uint16_t;
			static int getDb(char* s, size_t max, type v)
			{
				return snprintf(s, max, "%u", v);
			}
		};
		template<> struct _hap_type<FormatId::Uint32>
		{
			using type = uint32_t;
			static int getDb(char* s, size_t max, type v)
			{
				return snprintf(s, max, "%u", v);
			}
		};
		template<> struct _hap_type<FormatId::Uint64>
		{
			using type = uint64_t;
			static int getDb(char* s, size_t max, type v)
			{
				return snprintf(s, max, "%llu", v);
			}
		};
		template<> struct _hap_type<FormatId::Int>
		{
			using type = int32_t;
			static int getDb(char* s, size_t max, type v)
			{
				return snprintf(s, max, "%d", v);
			}
		};
		template<> struct _hap_type<FormatId::Float>
		{
			using type = double;
			static int getDb(char* s, size_t max, type v)
			{
				return snprintf(s, max, "%lg", v);
			}
		};
		template<> struct _hap_type<FormatId::ConstStr>
		{
			using type = const char *;
			static int getDb(char* s, size_t max, type v)
			{
				return snprintf(s, max, "\"%s\"", v);
			}
		};
		template<> struct _hap_type<FormatId::Format>
		{
			using type = FormatId;
			static int getDb(char* s, size_t max, type v)
			{
				return snprintf(s, max, "\"%s\"", FormatStr[int(v)]);
			}
		};
		template<> struct _hap_type<FormatId::Unit>
		{
			using type = UnitId;
			static int getDb(char* s, size_t max, type v)
			{
				return snprintf(s, max, "\"%s\"", UnitStr[int(v)]);
			}
		};
		template<> struct _hap_type<FormatId::String>
		{
			using type = char;
			static int getDb(char* str, size_t max, type v[], int _length)
			{
				char* s = str;
				int l = snprintf(s, max, "\"%.*s\"", _length, v);
				s += l;
				return s - str;
			}
		};
		template<> struct _hap_type<FormatId::Data>
		{
			using type = uint8_t;
		};
		template<> struct _hap_type<FormatId::IdArray>
		{
			using type = uint64_t;
			static int getDb(char* str, size_t max, type v[], int _length)
			{
				char* s = str;

				*s++ = '[';
				max--;

				bool comma = false;
				for (int i = 0; i < _length; i++)
				{
					if (comma)
					{
						*s++ = ',';
						max--;
					}
					int l = snprintf(s, max, "\"%lld\"", v[i]);
					s += l;
					max -= l;
					comma = true;
				}

				*s++ = ']';

				return s - str;
			}
		};

		// Hap::Property::Base - base class of Properties
		class Base
		{
		protected:
			KeyId _keyId;
			Base(KeyId keyId) : _keyId(keyId) {}
		public:
			KeyId keyId() const
			{
				return _keyId;
			}
			const char* key() const
			{
				return KeyStr[int(_keyId)];
			}

			virtual int getDb(char* str, size_t max)
			{
				return 0;
			}
		};

		// Hap::Property::Simple - base class for simple properties
		//	bool, int, uint, float, const string
		template<KeyId Key, FormatId Format>
		class Simple : public Base
		{
		public:
			static constexpr KeyId K = Key;
			using T = typename _hap_type<Format>::type;
		protected:
			T _v;
		public:
			Simple() : Base(Key) {}
			Simple(T v) : Base(Key), _v(v) {}

			T get() const { return _v; }
			void set(T v) { _v = v; }

			T& operator()()	{ return _v; }

			// get JSON-formatted characteristic descriptor
			virtual int getDb(char* str, size_t max) override
			{
				int l = snprintf(str, max, "\"%s\":", key());
				str += l;
				max -= l;
				l += _hap_type<Format>::getDb(str, max, _v);
				return l;
			}
		};

		// Hap::Property::Array - base class for array properties
		//	string, tlv8, data, linked services, valid values
		template<KeyId Key, FormatId Format, int Size>
		class Array : public Base
		{
		public:
			using T = typename _hap_type<Format>::type;
		protected:
			uint16_t _size = Size;	// max size of the array in elements
			uint16_t _length;		// current length of _v in elements
			T _v[Size];
		public:
			Array(const T* v = nullptr, uint16_t size = 0) : Base(Key)
			{
				if (v != nullptr)
					set(v, size);
				else
					_length = Size;
			}

			const T* get() const
			{
				return _v;
			}

			void set(const T* v, uint16_t length)
			{
				if (length > _size)
					length = _size;
				memcpy(_v, v, length * sizeof(T));
				_length = length;
			}

			T get(int i) const
			{
				if (i < _length)
					return _v[i];
				return T(-1);
			}

			void set(int i, T v)
			{
				if (i < _length)
					_v[i] = v;
			}

			// get JSON-formatted characteristic descriptor
			virtual int getDb(char* str, size_t max) override
			{
				int l = snprintf(str, max, "\"%s\":", key());
				str += l;
				max -= l;
				l += _hap_type<Format>::getDb(str, max, _v, _length);
				return l;
			}
		};

		// Properties
		using AccessoryInstanceId = Simple<KeyId::AccessoryInstanceId, FormatId::Uint64>;
		using Type = Simple<KeyId::Type, FormatId::ConstStr>;
		using InstanceId = Simple<KeyId::InstanceId, FormatId::Uint64>;
		using HiddenService = Simple<KeyId::HiddenServices, FormatId::Bool>;
		using PrimaryService = Simple<KeyId::PrimaryServices, FormatId::Bool>;
		template <uint8_t Size> using LinkedServices = Array<KeyId::LinkedServices, FormatId::IdArray, Size>;

		// Permissions - bitmask, special processing required
		class Permissions : public Simple<KeyId::Permissions, FormatId::Uint8>
		{
		public:
			enum Perm
			{
				PairedRead = 1 << 0,
				PairedWrite = 1 << 1,
				Events = 1 << 2,
				AdditionalAuthorization = 1 << 3,
				TimedWrite = 1 << 4,
				Hidden = 1 << 5
			};

			using Simple::Simple;

			bool isEnabled(Perm p)
			{
				return (get() & p) != 0;
			}

			virtual int getDb(char* str, size_t max) override
			{
				static const char* PermStr[] =
				{
					"pr", "pw", "ev", "aa", "tw", "hd"
				};
				char* s = str;

				int l = snprintf(s, max, "\"%s\":[", key());
				s += l;
				max -= l;

				bool comma = false;
				for (int i = 0; i < 5; i++)
				{
					if (isEnabled(Perm(1 << i)))
					{
						if (comma)
						{
							*s++ = ',';
							max--;
						}

						l = snprintf(s, max, "\"%s\"", PermStr[i]);
						s += l;
						max -= l;
						comma = true;
					}
				}

				*s++ = ']';

				return s - str;
			}
		};

		using EventNotifications = Simple<KeyId::EventNotifications, FormatId::Bool>;
		using Description = Simple<KeyId::Description, FormatId::ConstStr>;
		using Format = Simple<KeyId::Format, FormatId::Format>;
		using Unit = Simple<KeyId::Unit, FormatId::Unit>;
		template<FormatId F> using MinimumValue = Simple<KeyId::MinimumValue, F>;
		template<FormatId F> using MaximumValue = Simple<KeyId::MaximumValue, F>;
		template<FormatId F> using StepValue = Simple<KeyId::StepValue, F>;
		using MaxLength = Simple<KeyId::MaxLength, FormatId::Int>;
		using MaxDataLength = Simple<KeyId::MaxDataLength, FormatId::Int>;
		template<FormatId F, int S> using ValidValues = Array<KeyId::ValidValues, F, S>;
		template<FormatId F> using ValidValuesRange = Array<KeyId::ValidValuesRange, F, 2>;
	}

	namespace Characteristic
	{
		class Obj
		{
		public:
			virtual int getDb(char* str, size_t max) = 0;
		};

		// Hap::Characteristic::Base
		template<int PropCount>	// number of optional properties
		class Base : public Obj
		{
		private:
			static constexpr uint8_t _max = PropCount + 4;
			uint8_t _sz = 0;
			Property::Base* _pr[_max];
		protected:
			Property::Type _type;
			Property::InstanceId _iid;
			Property::Permissions _perms;
			Property::Format _format;

		public:
			void AddPr(Property::Base* ch)
			{
				if (_sz < _max)
					_pr[_sz++] = ch;
			}

			Property::Base* GetPr(int i)
			{
				if (i >= _sz)
					return nullptr;
				return _pr[i];
			}

			Base(
				Property::Type::T type,
				Property::InstanceId::T iid,
				Property::Permissions::T perms,
				Property::Format::T format
			) :
				_type(type),
				_iid(iid),
				_perms(perms),
				_format(format)
			{
				AddPr(&_type);
				AddPr(&_iid);
				AddPr(&_perms);
				AddPr(&_format);
			}

			// get JSON-formatted characteristic descriptor
			virtual int getDb(char* str, size_t max) override
			{
				char* s = str;

				if (max < 2)	// space for '{}'
					return -1;

				*s++ = '{';
				max--;

				bool comma = false;
				for (int i = 0; ; i++)
				{
					Property::Base* pr = GetPr(i);
					if (pr == nullptr)
						break;

					if (comma)
					{
						*s++ = ',';
						max--;
					}

					int l = pr->getDb(s, max);
					s += l;
					max -= l;
					comma = true;
				}

				*s++ = '}';

				return s - str;
			}

			// access to common properties
			Property::Type& Type() { return _type; }
			Property::InstanceId& Iid() { return _iid; }
			Property::Permissions& Perms() { return _perms; }
			Property::Format& Format() { return _format; }

			// access to all properties
			template<typename Prop> Prop* Property()
			{
				Property::KeyId key = Prop::K;
				for (int i = 0; i < _sz; i++)
				{
					Property::Base* pr = _pr[i];
					if (pr->keyId() == key)
						return static_cast<Prop*>(pr);
				}
				return nullptr;
			}
		};

		// Hap::Characteristic::Simple
		template<
			int PropCount,										// number of optional properties
			Property::FormatId F = Property::FormatId::Null		// format of the Value property
		>
			class Simple : public Base<PropCount + 1>
		{
		public:
			using T = Property::Simple<Property::KeyId::Value, F>;	// type of Value property
			using V = typename T::T;								// C type associated with T
		protected:
			T _value;
		public:
			Simple(Hap::Property::Type::T type, Hap::Property::InstanceId::T iid, Property::Permissions::T perms)
				: Base<PropCount + 1>(type, iid, perms, F)
			{
				Base<PropCount + 1>::AddPr(&_value);
			}

			// get/set the value
			V Value() { return _value.get(); }
			void Value(const V& value) { _value.set(value); }

			V& operator()() { return _value(); }
		};

		// Hap::Characteristic::Array
		template<
			int PropCount,			// number of optional properties
			Property::FormatId F,	// format of the Value property
			int Size = 64			// max size of the array (64 - default size for strings)
		>
			class Array : public Base<PropCount + 1>
		{
		public:
			using T = Property::Array<Property::KeyId::Value, F, Size>;	// type of Value property
			using V = typename T::T;									// C type associated with T (array base type)
		protected:
			T _value;
		public:
			Array(Hap::Property::Type::T type, Hap::Property::InstanceId::T iid, Property::Permissions::T perms)
				: Base<PropCount + 1>(type, iid, perms, F)
			{
				AddPr(&_value);
			}

			// get/set the value
			const V* Value() const { return _value.get(); }
			void Value(const V* v, uint16_t length) { _value.set(v, length); }
			V Value(int i) const { return _value.get(i); }
			void Value(int i, V v) { _value.set(i, v); }
		};
	}

	namespace Service
	{
		// Hap::Service::Base
		template<int CharCount>	// number of characteristics
		class Base
		{
		private:
			Property::Base* _pr[5];		// internal properties
			static constexpr uint8_t _max = CharCount;
			uint8_t _sz = 0;
			Characteristic::Obj* _ch[_max];

			void AddPr(Property::Base* pr, int i)
			{
				_pr[i] = pr;
			}

		protected:
			void AddType(Property::Base* pr) { AddPr(pr, 0); }
			void AddIid(Property::Base* pr) { AddPr(pr, 1); }
			void AddPrimary(Property::Base* pr) { AddPr(pr, 2); }
			void AddHidden(Property::Base* pr) { AddPr(pr, 3); }
			void AddLinked(Property::Base* pr) { AddPr(pr, 4); }

			Property::Base* GetPr(int i)
			{
				if (i >= 5)
					return nullptr;
				return _pr[i];
			}

			void AddCh(Characteristic::Obj* ch)
			{
				if (_sz < _max)
					_ch[_sz++] = ch;
			}

			Characteristic::Obj* GetCh(int i)
			{
				if (i >= _sz)
					return nullptr;
				return _ch[i];
			}
		};

		// Hap::Service::Service - base service class
		//	CharCount - max number of Characteristics the Service may contain
		template<int CharCount>
		class Service : public Base<CharCount>
		{
		protected:
			Property::Type _type;
			Property::InstanceId _iid;
			Property::PrimaryService _primary;
			Property::HiddenService _hidden;
		public:
			Service(
				Property::Type::T type,
				Property::InstanceId::T iid,
				Property::PrimaryService::T primary = false,
				Property::HiddenService::T hidden = false
			) :
				_type(type),
				_iid(iid),
				_primary(primary),
				_hidden(hidden)
			{
				Base<CharCount>::AddType(&_type);
				Base<CharCount>::AddIid(&_iid);
				Base<CharCount>::AddPrimary(&_primary);
				Base<CharCount>::AddHidden(&_hidden);
			}

			// access to properties
			Property::Type& Type() { return _type; }
			Property::InstanceId& Iid() { return _iid; }
			Property::HiddenService& Hidden() { return _hidden; }
			Property::PrimaryService& Primary() { return _primary; }

			// get JSON-formatted characteristic descriptor
			int getDb(char* str, size_t max)
			{
				char* s = str;
				int l;

				*s++ = '{';
				max--;

				for (int i = 0; ; i++)
				{
					Property::Base* pr = Base<CharCount>::GetPr(i);
					if (pr == NULL)
						break;

					l = pr->getDb(s, max);
					s += l;
					max -= l;
					*s++ = ',';
					max--;
				}

				l = snprintf(s, max, "\"characteristics\":[");
				s += l;
				max -= l;

				for (int i = 0; ; i++)
				{
					Characteristic::Obj* ch = Base<CharCount>::GetCh(i);
					if (ch == nullptr)
						break;

					l = ch->getDb(s, max);
					s += l;
					max -= l;
				}

				*s++ = ']';
				max--;

				*s++ = '}';

				return s - str;
			}
		};
	}
}

#endif
