#ifndef _HAP_APPLE_SERVICES_H_
#define _HAP_APPLE_SERVICES_H_

namespace Hap
{
	namespace Service
	{
		// Lightbulb
		//	This service describes a lightbulb.
		//	UUID 00000043-0000-1000-8000-0026BB765291
		//	Type public.hap.service.lightbulb
		//	Required Characteristics 
		//		On
		//	Optional Characteristics
		//		Brightness
		//		Hue
		//		Name
		//		Saturation
		//		Color Temperature
		// defines space for 6 characteristics, but only adds the On.
		//	custom extentions may add optonal ones 
		class Lightbulb : public Service<6>	// reserve space for 6 characteristics
		{
		protected:
			Characteristic::On _on;
		public:
			Lightbulb(
				Property::InstanceId::T iid,
				Characteristic::On::V v = false,
				Property::PrimaryService::T primary = false,
				Property::HiddenService::T hidden = false
			) : Service("43", iid, primary, hidden),
				_on(v)
			{
				AddCh(&_on);
				_on.Iid() = 6;

				auto p = _on.Property<Property::InstanceId>();
				p->set(8);

				Primary() = true;
			}

			// access to internal characteristics
			Characteristic::On& On() { return _on; }

		};
	}
}


#endif