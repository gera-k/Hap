#include "Hap.h"

namespace Hap
{

	bool Pairings::Add(const uint8_t* id, uint8_t id_len, const uint8_t* key, Controller::Perm perm)
	{
		for (int i = 0; i < sizeofarr(_db); i++)
		{
			Controller* rec = &_db[i];
			if (rec->perm == Controller::None)
			{
				// add new record
				memset(rec->id, 0, Controller::IdLen);
				memcpy(rec->id, id, id_len > Controller::IdLen ? Controller::IdLen : id_len);
				memcpy(rec->key, key, Controller::KeyLen);
				rec->perm = perm;

				return true;
			}
			else if (memcmp(rec->id, id, id_len) == 0)
			{
				// Id matches, TODO

				return true;
			}
		}

		return false;
	}

	bool Pairings::Add(const Hap::Tlv::Item& id, const Hap::Tlv::Item& key, Controller::Perm perm)
	{
		if (id.len() > Controller::IdLen)
		{
			Log("Pairings: Invalid ID length %d\n", id.len());
			return false;
		}

		if (key.len() != Controller::KeyLen)
		{
			Log("Pairings: Invalid key length %d\n", key.len());
			return false;
		}

		return Add(id.val(), id.len(), key.val(), perm);
	}

	const Controller* Pairings::Get(const Hap::Tlv::Item& id)
	{
		if (id.len() > Controller::IdLen)
			return nullptr;

		for (int i = 0; i < sizeofarr(_db); i++)
		{
			Controller* ios = &_db[i];

			if (memcmp(ios->id, id.val(), id.len()) == 0)
				return ios;
		}

		return nullptr;
	}
}

void Hex(const char* Header, const void* Buffer, size_t Length)
{
	static const char hex[] = "0123456789ABCDEF";
	const uint8_t* a = (const uint8_t*)Buffer;
	size_t i;
	size_t max = 16;

	Log("%s addr %p size 0x%X(%d):\n", Header, Buffer, (unsigned)Length, (unsigned)Length);

	while (Length > 0)
	{
		char line[52];
		char *p = line;

		if (Length < max)
			max = Length;

		memset(line, 0, sizeof(line));

		for (i = 0; i < 16; i++)
		{
			if (i < max)
			{
				*p++ = hex[(a[i] & 0xf0) >> 4];
				*p++ = hex[a[i] & 0x0f];
			}
			else
			{
				*p++ = ' ';
				*p++ = ' ';
			}
		}

		*p++ = ' ';
		*p++ = ' ';
		*p++ = ' ';

		for (i = 0; i < max; i++)
		{
			if (a[i] < 0x20 || a[i] > 0x7e) *p++ = '.';
			else *p++ = a[i];
		}

		Log("0x%04lX:%s\n", (uintptr_t)a & 0xFFFF, line);

		Length -= max;
		a += max;
	}
}
