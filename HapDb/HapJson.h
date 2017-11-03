#ifndef _HAP_JSON_H_
#define _HAP_JSON_H_

#include "jsmn.h"
#include <type_traits>
#include <limits>

namespace Hap
{
	namespace Json
	{
		struct member
		{
			const char* key;	// key
			uint8_t type;		// expected type mask, may include JSMN_UNDEFINED if key is optional

			// parse results:
			int i;				// i>0 - key is present, i = index of value; i<=0 - key not present
		};

		template<int TokenCount = 64>
		class Obj
		{
		private:
			jsmn_parser _ps;
			const char* _js;
			uint16_t _len;
			jsmntok_t _tk[TokenCount];
			int _cnt;

		public:
			Obj()
			{
				jsmn_init(&_ps);
			}

			// token pointer
			const jsmntok_t* tk(int i)
			{
				if (i < 0 || i >= TokenCount)
					return nullptr;
				return &_tk[i];
			}

			// token start
			const char* start(int i)
			{
				auto t = tk(i);
				if (t == nullptr)
					return nullptr;

				return _js + t->start;
			}

			// token length
			int length(int i)
			{
				auto t = tk(i);
				if (t == nullptr)
					return 0;

				return t->end - t->start;
			}

			// is this is the 'null' token?
			bool is_null(int i)
			{
				auto t = tk(i);
				if (t == nullptr)
					return false;

				if (cmp(t, "null", JSMN_PRIMITIVE))
					return true;
				
				return false;
			}

			// is this is the 'bool' token?
			bool is_bool(int i, bool& value)
			{
				auto t = tk(i);
				if (t == nullptr)
					return false;

				if (cmp(t, "true", JSMN_PRIMITIVE))
				{
					value = true;
					return true;
				}

				if (cmp(t, "false", JSMN_PRIMITIVE))
				{
					value = false;
					return true;
				}

				return false;
			}

			template<typename T>
			bool is_number(int i, T& value)
			{
				char s[24];
				auto t = tk(i);
				if (t == nullptr)
					return false;

				if constexpr (std::is_floating_point_v<T>)
				{
					double v = atof(copy(i, s, sizeofarr(s)));
					value = v;
					return true;
				}

				if constexpr (std::is_integral_v<T>)
				{
					if constexpr (std::numeric_limits<T>::is_signed)
					{
						int64_t v = atoll(copy(i, s, sizeofarr(s)));

						if (v >= std::numeric_limits<T>::min() && v <= std::numeric_limits<T>::max())
						{
							value = static_cast<T>(v);
							return true;
						}
					}
					else
					{
						uint64_t v = atoll(copy(i, s, sizeofarr(s)));

						if (v >= std::numeric_limits<T>::min() && v <= std::numeric_limits<T>::max())
						{
							value = static_cast<T>(v);
							return true;
						}
					}
				}

				return false;
			}

			// copy token i into buffer, zero terminate the buffer
			char* copy(int i, char* buf, size_t size)
			{
				size_t l = 0;
				const jsmntok_t* t = tk(i);
				if (t != nullptr)
				{
					l = t->end - t->start;
					if (l > size - 1)
						l = size - 1;
					strncpy_s(buf, size, _js + t->start, l);
				}
				buf[l] = 0;
				return buf;
			}

			// parse JSON-formatted string
			bool parse(const char* js, uint16_t len)
			{
				_js = js;
				_len = len;

				_cnt = jsmn_parse(&_ps, _js, _len, _tk, sizeofarr(_tk));

				return _cnt > 0;
			}

			// parse object
			// returns -1 when parsed ok, or index of first invalid or missing parameter
			int parse(int obj, member* om, int om_cnt)
			{
				int i;

				for (i = 0; i < om_cnt; i++)
				{
					member* m = &om[i];

					// find token that matches the requested key
					m->i = find(obj, m->key);

					if (m->i > 0)
					{
						m->i++;

						// verify that member type is valid
						if (!(m->type & _tk[m->i].type))
						{
							m->i = -1;
							return i;  // invalid type of key i
						}
					}
				}

				for (i = 0; i < om_cnt; i++)
				{
					member* m = &om[i];

					if (!(m->type & JSMN_UNDEFINED) && (m->i < 0))
						return i;	// missing mandatory key i 
				}

				return -1;
			}

			// find object <key> in json object <obj>
			//	returns token index or -1
			int find(int obj, const char* key)
			{
				if (_tk[obj].type != JSMN_OBJECT)
					return -1;

				int i = obj + 1;
				int cnt = _tk[obj].size;		// number of object members
				while (cnt)
				{
					// find next member which parent is obj
					if (_tk[i].parent == obj)
					{
						if (cmp(&_tk[i], key))
							return i;
						cnt--;
					}
					i++;  //assume t[obj].size is correct so we never go beyond the token array
				}
				return -1;
			}

			// find element <ind> of array <arr>
			//	returns token index or -1
			int find(int arr, int ind)
			{
				if (_tk[arr].type != JSMN_ARRAY)
					return -1;

				int i = arr + 1;
				int cnt = _tk[arr].size;		// number of array elements
				while (cnt)
				{
					// find next element which parent is arr
					if (_tk[i].parent == arr)
					{
						// if requested element is reached, return its index
						if (ind-- == 0)
						{
							return i;
						}
						cnt--;
					}
					i++;
				}
				return -1;
			}

			void dump()
			{
				dump(_tk, _cnt, 0);
			}

		private:
			bool cmp(const jsmntok_t *tk, const char *s, uint8_t type = JSMN_STRING)
			{
				int l = tk->end - tk->start;
				return tk->type & type
					&& (int)strlen(s) == l
					&& strncmp(_js + tk->start, s, l) == 0;
			}

			int dump(jsmntok_t *t, size_t count, int ind)
			{
				char indent[64];
				int i, j;
				if (count == 0)
				{
					return 0;
				}
				i = 0;
				j = ind;
				while (j-- > 0)
				{
					indent[i++] = ' ';
					indent[i++] = ' ';
				}
				indent[i++] = 0;

				if (t->type == JSMN_PRIMITIVE)
				{
					Log("%sP(%d-%d-%d)%.*s\n", indent, t->start, t->end, t->parent, t->end - t->start, _js + t->start);
					return 1;
				}
				else if (t->type == JSMN_STRING)
				{
					Log("%sS(%d-%d-%d)'%.*s'\n", indent, t->start, t->end, t->parent, t->end - t->start, _js + t->start);
					return 1;
				}
				else if (t->type == JSMN_OBJECT)
				{
					Log("%sO(%d-%d-%d %d)%.*s {\n", indent, t->start, t->end, t->parent, t->size, t->end - t->start, _js + t->start);
					j = 0;

					for (i = 0; i < t->size; i++)
					{
						j += dump(t + 1 + j, count - j, ind + 1);
						Log("%s  :\n", indent);
						j += dump(t + 1 + j, count - j, ind + 1);
					}

					Log("%s}\n", indent);
					return j + 1;
				}
				else if (t->type == JSMN_ARRAY)
				{
					j = 0;
					Log("%sA(%d-%d-%d %d)%.*s [\n", indent, t->start, t->end, t->parent, t->size, t->end - t->start, _js + t->start);

					for (i = 0; i < t->size; i++)
					{
						j += dump(t + 1 + j, count - j, ind + 1);
					}
					Log("%s]\n", indent);
					return j + 1;
				}
				return 0;
			}

		};
	}
}

#endif