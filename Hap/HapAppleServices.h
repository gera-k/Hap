#ifndef _HAP_APPLE_SERVICES_H_
#define _HAP_APPLE_SERVICES_H_

namespace Hap
{
	// Lightbulb
	//	This service describes a lightbulb.
	//	UUID 00000043-0000-1000-8000-0026BB765291
	//	Type public.hap.service.lightbulb
	//	Required Characteristics 
	//		0 On
	//	Optional Characteristics
	//		1 Brightness
	//		2 Hue
	//		3 Name
	//		4 Saturation
	//		5 Color Temperature
	// defines space for 6 characteristics, but only adds the On.
	//	custom extentions may add optonal ones 
	class Lightbulb : public Service<6>	// reserve space for 6 characteristics
	{
	protected:
		Characteristic::On _on;

		//void AddBrightness(Characteristic::Brightness& br) { AddChar(&br, 1); }

	public:
		static constexpr const char* Type = "43";

		Lightbulb(
			Characteristic::On::V v = false,
			Property::PrimaryService::T primary = false,
			Property::HiddenService::T hidden = false
		) : Service(Type),
			_on(v)
		{
			AddCharacteristic(&_on, 0);
		}

		// access to internal characteristics
		Characteristic::On& On() { return _on; }

	};
}


#endif