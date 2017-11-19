#ifndef _HAP_MDNS_H_
#define _HAP_MDNS_H_

// Interface to system-dependent MDNS implementation

namespace Hap
{
	class Mdns
	{
	protected:
		Hap::Config* _cfg;
		char _name[64];			// service name - initially set to _cfg.name
		char _txt[64];			// txt buffer
		uint8_t _txt_len = 0;

		void update()
		{
			char* p = _txt;
			int max = sizeof(_txt) - 1;
			char l;

			if (!_cfg->BCT)
			{
				strcpy(_name, _cfg->name);
			}
			else
			{
				strcpy(_name, "New - Bonjour Service Name");
			}

			// c# - current Configuration number
			//		updates only when accessory/service/characteristic is added or removed
			l = snprintf(p + 1, max,  "c#=%d", _cfg->cn);
			max -= l;
			if (max <= 0) goto Ret;
			*p++ = l;
			p += l;

			// ff - feature flags
			//		1 - supports HAP pairing
			l = snprintf(p + 1, max, "ff=1");
			max -= l;
			if (max <= 0) goto Ret;
			*p++ = l;
			p += l;

			// id - Device ID (randomly generated on each device reset)
			l = snprintf(p + 1, max, "id=%s", _cfg->id);
			max -= l;
			if (max <= 0) goto Ret;
			*p++ = l;
			p += l;

			// md - Model name
			l = snprintf(p + 1, max, "md=%s", _cfg->model);
			max -= l;
			if (max <= 0) goto Ret;
			*p++ = l;
			p += l;

			// pv - protocol version, requred if not 1.0

			// s# - current state number, must be 1
			l = snprintf(p + 1, max, "s#=1");
			max -= l;
			if (max <= 0) goto Ret;
			*p++ = l;
			p += l;

			// sf - status flags
			l = snprintf(p + 1, max, "sf=%u", _cfg->sf);
			max -= l;
			if (max <= 0) goto Ret;
			*p++ = l;
			p += l;

			// category identifier
			l = snprintf(p + 1, max, "ci=%d", _cfg->ci);
			max -= l;
			if (max <= 0) goto Ret;
			*p++ = l;
			p += l;
		
		Ret:
			_txt_len = p - _txt;
		}
	
	public:
		static Mdns* Create(Hap::Config* cfg);
		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual void Update() = 0;
	};
}

#endif