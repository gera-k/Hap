#include <stdio.h> 
#include <string>
#include <iostream>

#include "Hap.h"

#include <time.h>
#include <stdlib.h>
#include <stdarg.h>

// HAP logging functions
namespace Hap
{
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

			Log("0x%04lX:%s\n", (unsigned long)a & 0xFFFF, line);

			Length -= max;
			a += max;
		}
	}

	void Log(const char* f, ...)
	{
		va_list arg;
		va_start(arg, f);

		vprintf(f, arg);
	}
}

#define Log Hap::Log

// random number generator
extern "C" {
	void t_stronginitrand()
	{
		srand((unsigned)time(NULL));
	}

	void t_random(unsigned char* data, unsigned size)
	{

		for (unsigned i = 0; i < size; i++)
		{
			*data++ = rand() & 0xFF;
		}
	}
}

