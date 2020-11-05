#ifndef _CLOUD_HOME_ASSISTANT_H_
#define _CLOUD_HOME_ASSISTANT_H_


namespace hass {

	enum struct DeviceType {
		BinarySensor,
		Sensor,
		Switch,
	};

	class Device {
	public:
	private:


	};


	class Connector {
	public:
		Connector(const String& ServerHost, const String& hassPrefix);
		
	private:
	};

}

#endif //_CLOUD_HOME_ASSISTANT_H_
