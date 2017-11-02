#ifndef _HAP_JSON_H_
#define _HAP_JSON_H_

#include "jsmn.h"

namespace Hap
{
	namespace Json
	{
		template<int TokenCount = 64>
		class Parser
		{
		private:
			jsmn_parser _ps;
			const char* _js;
			uint16_t _len;
			jsmntok_t _tk[TokenCount];
			int _cnt;

		public:
			Parser()
			{
				jsmn_init(&_ps);
			}

			bool parse(const char* js, uint16_t len)
			{
				_js = js;
				_len = len;

				_cnt = jsmn_parse(&_ps, _js, _len, _tk, sizeofarr(_tk));

				return _cnt > 0;
			}

			// find object <key> in json object <obj>
			//	returns token index or -1
			static int json_find_key(int obj, const char* key)
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
						if (cmp(&tk[i], key) == 0)
						{
							return i;
						}
						cnt--;
					}
					i++;  //assume t[obj].size is correct so we never go beyond the token array
				}
				return -1;
			}

			// find element <ind> of array <arr>
			//	returns token index or -1
			int find_elm(int arr, int ind)
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
			bool cmp(jsmntok_t *tk, const char *s)
			{
				int l = tk->end - tk->start;
				return tk->type == JSMN_STRING
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