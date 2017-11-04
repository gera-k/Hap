#ifndef _HAP_DB_H_
#define _HAP_DB_H_

namespace Hap
{
	using iid_t = uint32_t;		// limit instance IDs range
	constexpr iid_t null_id = 0;

	// Hap operation status
	enum class Status : uint8_t
	{
		Success = 0,
		RequestDenied,
		UnableToCommunicate,
		ResourceIsBusy,
		CannotWrite,
		CannotRead,
		NotificationNotSupported,
		OutOfResources,
		OperationTimedOut,
		ResourceNotExist,
		InvalidValue,
		InsufficientAuthorization
	};
	static const char* StatusStr(Status c)
	{
		static const char* const str[] =
		{
			"0",
			"-70401",
			"-70402",
			"-70403",
			"-70404",
			"-70405",
			"-70406",
			"-70407",
			"-70408",
			"-70409",
			"-70410",
			"-70411",
		};
		return str[int(c)];
	}

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

		// variable-size properties
		String,			// char[S]
		Tlv8,			// uint8_t[S]
		Data,			// uint8_t[S]

		Id,				// iid_t
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
		nullptr,
		nullptr
	};

	// this set of templates maps FormatId to:
	//	- C type used for internal representation
	//	- getDb function to format a property to JSON
	//	- Write function to convert JSON token to internal representation
	template <FormatId> struct hap_type;
	template<> struct hap_type<FormatId::Null>
	{
		using type = uint8_t;
		static inline int getDb(char* s, size_t max, type v)
		{
			return snprintf(s, max, "null");
		}
		static inline bool Write(const Hap::Json::Obj& js, int t, type& v)
		{
			return js.is_null(t);
		}
	};
	template<> struct hap_type<FormatId::Bool>
	{
		using type = bool;
		static inline int getDb(char* s, size_t max, type v)
		{
			return snprintf(s, max, "%s", v ? "true" : "false");
		}
		static inline bool Write(const Hap::Json::Obj& js, int t, type& v)
		{
			return js.is_bool(t, v);
		}
	};
	template<> struct hap_type<FormatId::Uint8>
	{
		using type = uint8_t;
		static inline int getDb(char* s, size_t max, type v)
		{
			return snprintf(s, max, "%u", v);
		}
		static inline bool Write(const Hap::Json::Obj& js, int t, type& v)
		{
			return js.is_number<type>(t, v);
		}
	};
	template<> struct hap_type<FormatId::Uint16>
	{
		using type = uint16_t;
		static inline int getDb(char* s, size_t max, type v)
		{
			return snprintf(s, max, "%u", v);
		}
		static inline bool Write(const Hap::Json::Obj& js, int t, type& v)
		{
			return js.is_number<type>(t, v);
		}
	};
	template<> struct hap_type<FormatId::Uint32>
	{
		using type = uint32_t;
		static inline int getDb(char* s, size_t max, type v)
		{
			return snprintf(s, max, "%u", v);
		}
		static inline bool Write(const Hap::Json::Obj& js, int t, type& v)
		{
			return js.is_number<type>(t, v);
		}
	};
	template<> struct hap_type<FormatId::Uint64>
	{
		using type = uint64_t;
		static inline int getDb(char* s, size_t max, type v)
		{
			return snprintf(s, max, "%llu", v);
		}
		static inline bool Write(const Hap::Json::Obj& js, int t, type& v)
		{
			return js.is_number<type>(t, v);
		}
	};
	template<> struct hap_type<FormatId::Int>
	{
		using type = int32_t;
		static inline int getDb(char* s, size_t max, type v)
		{
			return snprintf(s, max, "%d", v);
		}
		static inline bool Write(const Hap::Json::Obj& js, int t, type& v)
		{
			return js.is_number<type>(t, v);
		}
	};
	template<> struct hap_type<FormatId::Float>
	{
		using type = double;
		static inline int getDb(char* s, size_t max, type v)
		{
			return snprintf(s, max, "%lg", v);
		}
		static inline bool Write(const Hap::Json::Obj& js, int t, type& v)
		{
			return js.is_number<type>(t, v);
		}
	};
	template<> struct hap_type<FormatId::ConstStr>
	{
		using type = const char *;
		static inline int getDb(char* s, size_t max, type v)
		{
			return snprintf(s, max, "\"%s\"", v);
		}
	};
	template<> struct hap_type<FormatId::Format>
	{
		using type = FormatId;
		static inline int getDb(char* s, size_t max, type v)
		{
			return snprintf(s, max, "\"%s\"", FormatStr[int(v)]);
		}
	};
	template<> struct hap_type<FormatId::Unit>
	{
		using type = UnitId;
		static inline int getDb(char* s, size_t max, type v)
		{
			return snprintf(s, max, "\"%s\"", UnitStr[int(v)]);
		}
	};
	template<> struct hap_type<FormatId::String>
	{
		using type = char;
		static inline int getDb(char* str, size_t max, type v[], int _length)
		{
			char* s = str;
			int l = snprintf(s, max, "\"%.*s\"", _length, v);
			s += l;

			return s - str;
		}
	};
	template<> struct hap_type<FormatId::Data>
	{
		using type = uint8_t;
	};
	template<> struct hap_type<FormatId::Id>
	{
		using type = iid_t;
		static inline int getDb(char* s, size_t max, type v)
		{
			return snprintf(s, max, "%u", v);
		}
	};
	template<> struct hap_type<FormatId::IdArray>
	{
		using type = uint64_t;
		static int getDb(char* str, int max, type v[], int _length)
		{
			char* s = str;
			bool comma;

			*s++ = '[';
			max--;
			if (max <= 0) goto Ret;

			comma = false;
			for (int i = 0; i < _length; i++)
			{
				if (comma)
				{
					*s++ = ',';
					max--;
					if (max <= 0) goto Ret;
				}
				int l = snprintf(s, max, "\"%lld\"", v[i]);
				s += l;
				max -= l;
				if (max <= 0) goto Ret;

				comma = true;
			}

			*s++ = ']';
		Ret:
			return s - str;
		}
	};

	// Obj - base class for most of DB objects
	//	defines set of virtual functions
	class Obj
	{
	public:
		virtual int getDb(char* str, int max) = 0;
		virtual iid_t getId() { return null_id; }

		struct wr_prm
		{
			Hap::Json::Obj& rq;			// request object

			Hap::iid_t aid;
			Hap::iid_t iid;

			bool val_present = false;	// value member is present
			uint8_t val_ind = 0;		// value token index in rq
			bool ev_present = false;	// event member is present
			bool ev_value = false;		// event member value
			bool auth_present = false;	// authData member is present 
			uint8_t auth_ind = 0;		// authData token index in rq
			bool remote_present = false;// remote member is present
			bool remote_value = false;	// remote member value

			Hap::Status status = Hap::Status::Success;	// write status
		};

		// Write returns true when it completes write to characteristic, 
		//	status of the operation is indicated in p.status
		virtual bool Write(wr_prm& p) { return false; };
	};

	// array of DB objects
	//	max size is set on compile time through Count parameter
	template<int Count>
	class ObjArray
	{
	private:
		uint8_t _sz = 0;
		Obj* _obj[Count];
	
	public:
		// return current size of the array
		uint8_t size() const
		{
			return _sz;
		}
		
		// add object to the end of the array
		void set(Obj* obj)
		{
			if (_sz < Count)
				_obj[_sz++] = obj;
		}
		
		// add object to position i
		void set(Obj* obj, int i)
		{
			if (i < Count)
			{
				_obj[i] = obj;
				if (_sz <= i)
					_sz = i + 1;
			}
		}
		
		// get object at position i
		Obj* get(int i) const
		{
			if (i >= _sz)
				return nullptr;
			return _obj[i];
		}
		
		// get(iid_t) - seeks object by object ID
		Obj* GetObj(iid_t id)
		{
			for (int i = 0; i < _sz; i++)
			{
				Obj* obj = _obj[i];
				if (obj == nullptr)
					continue;
				if (obj->getId() == id)
					return obj;
			}
			return nullptr;
		}

		// getDb - create JSON representation of the array
		int getDb(char* str, int max, const char* name = nullptr) const
		{
			char* s = str;
			int l;
			bool comma;

			if (max <= 0) goto Ret;

			if (name != nullptr)
			{
				l = snprintf(s, max, "\"%s\":[", name);
				s += l;
				max -= l;
				if (max <= 0) goto Ret;
			}

			comma = false;
			for (int i = 0; i < _sz; i++)
			{
				Obj* obj = _obj[i];
				if (obj != nullptr)
				{
					if (comma)
					{
						*s++ = ',';
						max--;
						if (max <= 0) goto Ret;
					}

					l = obj->getDb(s, max);
					s += l;
					max -= l;
					if (max <= 0) goto Ret;
					comma = true;
				}
			}

			if (name != nullptr)
			{
				*s++ = ']';
				max--;
				if (max <= 0) goto Ret;
			}
		Ret:
			return s - str;
		}
	};

	namespace Property
	{
		// Hap::Property::Obj - base class of Properties
		class Obj : public Hap::Obj
		{
		protected:
			KeyId _keyId;
			Obj(KeyId keyId) : _keyId(keyId) {}
		public:
			KeyId keyId() const
			{
				return _keyId;
			}
			const char* key() const
			{
				return KeyStr[int(_keyId)];
			}
		};

		// Hap::Property::Simple - base class for simple properties
		//	bool, int, uint, float, const string
		template<KeyId Key, FormatId Format>
		class Simple : public Obj
		{
		public:
			static constexpr KeyId K = Key;
			using T = typename hap_type<Format>::type;
		protected:
			T _v;
		public:
			Simple() : Obj(Key) {}
			Simple(T v) : Obj(Key), _v(v) {}

			T get() const { return _v; }
			void set(T v) { _v = v; }

			// get JSON-formatted characteristic descriptor
			virtual int getDb(char* str, int max) override
			{
				char* s = str;
				int l;
					
				if (max <= 0) goto Ret;

				l = snprintf(s, max, "\"%s\":", key());
				s += l;
				max -= l;
				if (max <= 0) goto Ret;

				l = hap_type<Format>::getDb(s, max, _v);
				s += l;
				max -= l;
			Ret:
				return s - str;
			}
		};

		// Hap::Property::Array - base class for array properties
		//	string, tlv8, data, linked services, valid values
		template<KeyId Key, FormatId Format, int Size>
		class Array : public Obj
		{
		public:
			using T = typename hap_type<Format>::type;
		protected:
			uint16_t _size = Size;	// max size of the array in elements
			uint16_t _length;		// current length of _v in elements
			T _v[Size];
		public:
			Array(const T* v = nullptr, uint16_t size = 0) : Obj(Key)
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
			virtual int getDb(char* str, int max) override
			{
				char*s = str;
				int l;
					
				if (max <= 0) goto Ret;

				l = snprintf(s, max, "\"%s\":", key());
				s += l;
				max -= l;
				if (max <= 0) goto Ret;

				l = hap_type<Format>::getDb(s, max, _v, _length);
				s += l;
				max -= l;

			Ret:
				return s - str;
			}
		};

		// Properties
		using AccessoryInstanceId = Simple<KeyId::AccessoryInstanceId, FormatId::Id>;
		using Type = Simple<KeyId::Type, FormatId::ConstStr>;
		using InstanceId = Simple<KeyId::InstanceId, FormatId::Id>;
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

			virtual int getDb(char* str, int max) override
			{
				static const char* PermStr[] =
				{
					"pr", "pw", "ev", "aa", "tw", "hd"
				};
				char* s = str;
				bool comma;
				int l;

				if (max <= 0) goto Ret;

				l = snprintf(s, max, "\"%s\":[", key());
				s += l;
				max -= l;
				if (max <= 0) goto Ret;

				comma = false;
				for (int i = 0; i < 5; i++)
				{
					if (isEnabled(Perm(1 << i)))
					{
						if (comma)
						{
							*s++ = ',';
							max--;
							if (max <= 0) goto Ret;
						}

						l = snprintf(s, max, "\"%s\"", PermStr[i]);
						s += l;
						max -= l;
						if (max <= 0) goto Ret;

						comma = true;
					}
				}

				*s++ = ']';
			Ret:
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
		// Hap::Characteristic::Base
		template<int PropCount>	// number of optional properties
		class Base : public Obj
		{
		private:
			ObjArray<PropCount + 4> _prop;	// first four slots are for mandatory properties:
		
			Property::Type _type;
			Property::InstanceId _iid;
			Property::Permissions _perms;
			Property::Format _format;

			void AddProp(Obj* pr, int i) { _prop.set(pr, i); }

		protected:
			void AddProp(Obj* pr) { _prop.set(pr); }
//			Parser* GetProp(int i) { return _prop.get(i); }

		public:
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
				AddProp(&_type, 0);
				AddProp(&_iid, 1);
				AddProp(&_perms, 2);
				AddProp(&_format, 3);
			}

			// get JSON-formatted characteristic descriptor
			virtual int getDb(char* str, int max) override
			{
				char* s = str;
				int l;

				if (max <= 0) goto Ret;

				*s++ = '{';
				max--;
				if (max <= 0) goto Ret;

				l = _prop.getDb(s, max);
				s += l;
				max -= l;
				if (max <= 0) goto Ret;

				*s++ = '}';
			Ret:
				return s - str;
			}

			virtual iid_t getId() override
			{ 
				return _iid.get(); 
			} 

			// access to common properties
			Property::Type& Type() { return _type; }
			Property::InstanceId& Iid() { return _iid; }
			Property::Permissions& Perms() { return _perms; }
			Property::Format& Format() { return _format; }

			// access to all properties
			template<typename Prop> Prop* GetProp()
			{
				KeyId key = Prop::K;
				for (int i = 0; i < _prop.size(); i++)
				{
					Property::Obj* pr = static_cast<Property::Obj*>(_prop.get(i));
					if (pr->keyId() == key)
						return static_cast<Prop*>(pr);
				}
				return nullptr;
			}
		};

		// Hap::Characteristic::Simple
		template<
			int PropCount,					// number of optional properties
			FormatId F = FormatId::Null		// format of the Value property
		>
		class Simple : public Base<PropCount + 1>
		{
		public:
			using T = Property::Simple<KeyId::Value, F>;	// type of Value property
			using V = typename T::T;						// C type associated with T
		protected:
			T _value;
		public:
			Simple(Hap::Property::Type::T type, Hap::Property::InstanceId::T iid, Property::Permissions::T perms)
				: Base<PropCount + 1>(type, iid, perms, F)
			{
				Base<PropCount + 1>::AddProp(&_value);
			}

			// get/set the value
			V Value() { return _value.get(); }
			void Value(const V& value) { _value.set(value); }

			virtual bool Write(Obj::wr_prm& p) override
			{ 
				if (p.iid != Iid().get())
				{
					p.status = Hap::Status::ResourceNotExist;
					return false;
				}

				// if ev present, set it first
				if (p.ev_present)
				{
					Property::EventNotifications* ev = GetProp<Property::EventNotifications>();
					if (ev == nullptr)
					{
						p.status = Hap::Status::ResourceNotExist;
					}
					else if (!Perms().isEnabled(Property::Permissions::Events))
					{
						p.status = Hap::Status::NotificationNotSupported;
					}
					else
					{
						ev->set(p.ev_value);
					}
				}

				// if value is present, set it
				if (p.val_present)
				{
					if (!Perms().isEnabled(Property::Permissions::PairedWrite))
					{
						p.status = Hap::Status::CannotWrite;
					}
					else
					{
						V v;
						bool rc = hap_type<F>::Write(p.rq, p.val_ind, v);

						if (rc)
						{
							_value.set(v);
						}
						else
						{
							p.status = Hap::Status::InvalidValue;
						}
					}
				}

				return true;	// true indicates that characteristic was found
			};
		};

		// Hap::Characteristic::Array
		template<
			int PropCount,			// number of optional properties
			FormatId F,				// format of the Value property
			int Size = 64			// max size of the array (64 - default size for strings)
		>
		class Array : public Base<PropCount + 1>
		{
		public:
			using T = Property::Array<KeyId::Value, F, Size>;	// type of Value property
			using V = typename T::T;							// C type associated with T (array base type)
		protected:
			T _value;
		public:
			Array(Hap::Property::Type::T type, Hap::Property::InstanceId::T iid, Property::Permissions::T perms)
				: Base<PropCount + 1>(type, iid, perms, F)
			{
				AddProp(&_value);
			}

			// get/set the value
			const V* Value() const { return _value.get(); }
			void Value(const V* v, uint16_t length) { _value.set(v, length); }
			V Value(int i) const { return _value.get(i); }
			void Value(int i, V v) { _value.set(i, v); }
		};
	}

	// Hap::Service
	template<int CharCount>	// max number of characteristics
	class Service : public Obj
	{
	private:
		ObjArray<5> _prop;						// internal properties, slot [4] is for optional Linked property

		Property::Type _type;
		Property::InstanceId _iid;
		Property::PrimaryService _primary;
		Property::HiddenService _hidden;

		void AddProp(Obj* pr, int i) { _prop.set(pr, i); }
//			Parser* GetProp(int i) { return _prop.get(i); }

		ObjArray<CharCount> _char;	// characteristics

	protected:
		void AddLinked(Property::Obj* linked) {	AddProp(linked, 4); }

		void AddChar(Obj* ch) { _char.set(ch); }
		Obj* GetChar(int i) { return _char.get(i); }

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
			AddProp(&_type, 0);
			AddProp(&_iid, 1);
			AddProp(&_primary, 2);
			AddProp(&_hidden, 3);
		}

		// access to properties
		Property::Type& Type() { return _type; }
		Property::InstanceId& Iid() { return _iid; }
		Property::HiddenService& Hidden() { return _hidden; }
		Property::PrimaryService& Primary() { return _primary; }

		// get JSON-formatted characteristic descriptor
		virtual int getDb(char* str, int max) override
		{
			char* s = str;
			int l;

			if (max <= 0) goto Ret;

			*s++ = '{';
			max--;
			if (max <= 0) goto Ret;

			l = _prop.getDb(s, max);
			s += l;
			max -= l;
			if (max <= 0) goto Ret;

			*s++ = ',';
			max--;
			if (max <= 0) goto Ret;

			l = _char.getDb(s, max, "characteristics");
			s += l;
			max -= l;
			if (max <= 0) goto Ret;

			*s++ = '}';
		Ret:
			return s - str;
		}

		virtual iid_t getId() override
		{
			return _iid.get();
		}

		virtual bool Write(wr_prm& p) override
		{
			for (int i = 0; i < _char.size(); i++)
			{
				auto ch = GetChar(i);
				if (ch == nullptr)
					continue;

				if (ch->getId() == p.iid)
				{
					return ch->Write(p);
				}
			}

			return false;
		}

	};

	// Hap::Accesory
	template<int ServCount>	// number of services
	class Accessory : public Obj
	{
	private:
		ObjArray<1> _prop;
		ObjArray<ServCount> _serv;

		// internal properties
		Property::AccessoryInstanceId _aid;

		void AddProp(Obj* pr) { _prop.set(pr); }
//			Parser* GetProp(int i) { return _prop.get(i); }

	protected:
		void AddServ(Obj* serv) { _serv.set(serv); }
		Obj* GetServ(int i) { return _serv.get(i); }

	public:
		Accessory(Property::AccessoryInstanceId::T aid = 0) 
		:	_aid(aid)
		{
			AddProp(&_aid);
		}

		// access to properties
		Property::AccessoryInstanceId& Aid() { return _aid; }

		// get/set accessory ID
		Property::AccessoryInstanceId::T aid()
		{
			return _aid.get();
		}
		void aid(Property::AccessoryInstanceId::T aid)
		{
			_aid.set(aid);
		}

		// get JSON-formatted descriptor
		virtual int getDb(char* str, int max) override
		{
			char* s = str;
			int l;

			if (max <= 0) goto Ret;

			*s++ = '{';
			max--;
			if (max <= 0) goto Ret;

			l = _prop.getDb(s, max);
			s += l;
			max -= l;
			if (max <= 0) goto Ret;

			*s++ = ',';
			max--;
			if (max <= 0) goto Ret;

			l = _serv.getDb(s, max, "services");
			s += l;
			max -= l;
			if (max <= 0) goto Ret;

			*s++ = '}';
		Ret:
			return s - str;
		}

		virtual iid_t getId() override
		{
			return _aid.get();
		}

		virtual bool Write(wr_prm& p) override
		{
			if (p.aid != _aid.get())
			{
				p.status = Hap::Status::ResourceNotExist;
				return false;
			}

			// pass Write to each Service until	it returns true
			for (int i = 0; i < _serv.size(); i++)
			{
				auto serv = GetServ(i);
				if (serv == nullptr)
					continue;

				bool rc = serv->Write(p);
				if (rc)
					return true;
			}

			return false;
		}
	};

	// Hap::Db
	template<int AccCount>		// max number of Accessories
	class Db
	{
	private:
		ObjArray<AccCount> _acc;

	protected:
		void AddAcc(Obj* acc) {	_acc.set(acc); }
		Obj* GetAcc(iid_t id) { return _acc.GetObj(id); }

	public:
		enum Status
		{
			HTTP_200,
			HTTP_204,
			HTTP_207,
			HTTP_400,
			HTTP_404,
			HTTP_422,
			HTTP_500,
			HTTP_503
		};
		static const char* StatusStr(Status c)
		{
			static const char* const str[] =
			{
				"200 OK",
				"204 No Content",
				"207 Multi-Status",
				"400 Bad Request",
				"404 Not Found",
				"422 Unprocessable Entry",
				"500 Internal Server Error",
				"503 Service Unavailable",
			};
			return str[int(c)];
		}

		Db()
		{
		}

		// get JSON-formatted database
		int getDb(char* str, int max)
		{
			char* s = str;
			int l;

			if (max <= 0) goto Ret;

			*s++ = '{';
			max--;
			if (max <= 0) goto Ret;

			l = _acc.getDb(s, max, "accessories");
			s += l;
			max -= l;

			*s++ = '}';
		Ret:
			return s - str;
		}

		// exec HTTP write request
		//	accepts JSON-formatted message body of parsed HTTP request
		//	returns HTTP status and JSON-formatted body of HTTP response
		Status Write(const char* req, int req_length, char* rsp, int rsp_size)
		{
			Hap::Json::Parser<> wr;
			int rc = wr.parse(req, req_length);

			Log("parse = %d\n", rc);

			// expect root object
			if (!rc || wr.tk(0)->type != Hap::Json::JSMN_OBJECT)
			{
				Log("JSON parse error\n");
				return HTTP_400;	// Bad request
			}

			wr.dump();

			// parse root object
			Hap::Json::member om[] =
			{
				{ "characteristics", Hap::Json::JSMN_ARRAY }
			};
			rc = wr.parse(0, om, sizeofarr(om));
			if (rc >= 0)
			{
				Log("parameter '%s' is missing or invalid", om[rc].key);
				return HTTP_400;
			}

			int cnt = wr.tk(om[0].i)->size;
			Log("Request contains %d characteristics\n", cnt);

			// prepare response
			char* s = rsp;
			int l, max = rsp_size;
			bool comma = false;
			int errcnt = 0;

			l = snprintf(s, max, "{\"characteristics\":[");
			s += l;
			max -= l;
			if (max <= 0)
				return HTTP_500;	// Internal error

			// parse and execute individual writes
			for (int i = 0; i < cnt; i++)
			{
				int c = wr.find(om[0].i, i);

				if (c < 0)
				{
					Log("Characteristic %d not found\n", i);
					return HTTP_400;
				}

				if (wr.tk(c)->type != Hap::Json::JSMN_OBJECT)
				{
					Log("Characteristic %d: Object expected\n", i);
					return HTTP_400;
				}

				// parse characteristic object
				Hap::Json::member om[] =
				{
					{ "aid", Hap::Json::JSMN_PRIMITIVE },
					{ "iid", Hap::Json::JSMN_PRIMITIVE },
					{ "value", Hap::Json::JSMN_ANY | Hap::Json::JSMN_UNDEFINED },
					{ "ev", Hap::Json::JSMN_PRIMITIVE | Hap::Json::JSMN_UNDEFINED },
					{ "authData", Hap::Json::JSMN_STRING | Hap::Json::JSMN_UNDEFINED },
					{ "remote", Hap::Json::JSMN_PRIMITIVE | Hap::Json::JSMN_UNDEFINED },
				};

				rc = wr.parse(c, om, sizeofarr(om));
				if (rc >= 0)
				{
					Log("Characteristic %d: parameter '%s' is missing or invalid", i, om[rc].key);
					return HTTP_400;
				}

				// fill write request parameters and status
				Obj::wr_prm p = { wr };

				// aid
				if (!wr.is_number<Hap::iid_t>(om[0].i, p.aid))
				{
					Log("Characteristic %d: invalid aid\n", i);
					return HTTP_400;
				}

				// iid
				if (!wr.is_number<Hap::iid_t>(om[1].i, p.iid))
				{
					Log("Characteristic %d: invalid iid\n", i);
					return HTTP_400;
				}

				// value
				if (om[2].i > 0)
				{
					p.val_present = true;
					p.val_ind = om[2].i;
				}

				// ev
				if (om[3].i > 0)
				{
					p.ev_present = wr.is_bool(om[3].i, p.ev_value);
				}

				// authData
				if (om[4].i > 0)
				{
					p.auth_present = true;
					p.auth_ind = om[4].i;
				}

				// remote
				if (om[5].i > 0)
				{
					p.remote_present = wr.is_bool(om[5].i, p.remote_value);
				}

				Log("Characteristic %d:  aid %u  iid %u\n", i, p.aid, p.iid);
				if (p.val_present)
					Log("      value: '%.*s'\n", wr.length(p.val_ind), wr.start(p.val_ind));
				if (p.ev_present)
					Log("         ev: %s\n", p.ev_value ? "true" : "false");
				if (p.auth_present)
					Log("   authData: '%.*s'\n", wr.length(p.auth_ind), wr.start(p.auth_ind));
				if (p.remote_present)
					Log("         ev: %s\n", p.remote_value ? "true" : "false");

				// find accessory by aid
				auto acc = GetAcc(p.aid);
				if (acc == nullptr)
				{
					p.status = Hap::Status::ResourceNotExist;
				}
				else
				{
					acc->Write(p);
				}
			}

			return HTTP_200;
		}
	};

}

#endif
