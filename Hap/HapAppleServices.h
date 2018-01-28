#ifndef _HAP_APPLE_SERVICES_H_
#define _HAP_APPLE_SERVICES_H_

namespace Hap
{
	// Accessory Information
	//	
	//	UUID 0000003E-0000-1000-8000-0026BB765291
	//	Type public.hap.service.accessory-information
	//	Required Characteristics 
	//		0 Identify
	//		1 Manufacturer
	//		2 Model
	//		3 Name
	//		4 Serial Number
	//		5 Firmware Revision
	//	Optional Characteristics
	//		6 Hardware Revision
	//		7 Accessory Flags
	// defines space for 8 characteristics and adds Required ones.
	//	custom extentions may add optonal ones 
	class AccessoryInformation : public Service<8>	// reserve space for 8 characteristics
	{
	protected:
		Characteristic::Identify _identify;
		Characteristic::Manufacturer _manufacturer;
		Characteristic::Model _model;
		Characteristic::Name _name;
		Characteristic::SerialNumber _serialNumber;
		Characteristic::FirmwareRevision _firmwareRevision;

		void Add(Characteristic::HardwareRevision& ch) { AddCharacteristic(&ch, 6); }

	public:
		static constexpr const char* Type = "3E";

		AccessoryInformation(
			Property::PrimaryService::T primary = false,
			Property::HiddenService::T hidden = false
		) : Service(Type)
		{
			AddCharacteristic(&_identify, 0);
			AddCharacteristic(&_manufacturer, 1);
			AddCharacteristic(&_model, 2);
			AddCharacteristic(&_name, 3);
			AddCharacteristic(&_serialNumber, 4);
			AddCharacteristic(&_firmwareRevision, 5);
		}
	};

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
	// defines space for 6 characteristics and adds Required ones.
	//	custom extentions may add optonal ones 
	class Lightbulb : public Service<6>	// reserve space for 6 characteristics
	{
	protected:
		Characteristic::On _on;

		void AddBrightness(Characteristic::Brightness& ch) { AddCharacteristic(&ch, 1); }
//		void AddHue(Characteristic::Hue& ch) { AddCharacteristic(&ch, 2); }
		void AddName(Characteristic::Name& ch) { AddCharacteristic(&ch, 3); }
//		void AddSaturation(Characteristic::Saturation& ch) { AddCharacteristic(&ch, 4); }
//		void AddColorTemp(Characteristic::ColorTemp& ch) { AddCharacteristic(&ch, 5); }

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
	};
}


#endif
