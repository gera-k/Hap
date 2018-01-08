#include "Hap.cpp"
#include "HapCrypt.cpp"
#include "HapSrp.cpp"
#include "HapHttp.cpp"
#include "jsmn.cpp"
#include "picohttpparser.cpp"

extern "C" {
	void t_stronginitrand()
	{
		srand((unsigned)time(NULL));
	}

	void t_random(unsigned char* data, unsigned size)
	{

		for (unsigned i = 0; i < size; i++)
		{
			*data++ = /*rand()*/ i & 0xFF;
		}
	}
}

