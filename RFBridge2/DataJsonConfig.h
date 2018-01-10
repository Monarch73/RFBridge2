#pragma once

#include <ArduinoJson.h>
#include "DataJsonInterface.h"

////{
////"name":"hue emulator",
////"swversion" : "81012917",
////"bridgeid" : "##HUEID##",
////"portalservices" : false,
////"linkbutton" : false,
////"mac" : "##MAC##",
////"dhcp" : true,
////"ipaddress" : "##IP##",
////"netmask" : "255.255.255.0",
////"gateway" : "192.168.1.1",
////"apiversion" : "1.3.0",
////"localtime" : "2017-12-03T12:07:32",
////"timezone" : "Europe/London",
////"whitelist" : {"api":{"name":"clientname#devicename"}},
////"swupdate" : {"text":"", "notify" : false, "updatestate" : 0, "url" : ""}
////  }

class DataJsonConfig : public DataJsonInterface
{

private:
	char *_bridgeId;
	String _mac;
	String _iPAdress;

public:
	DataJsonConfig(char *bridgeId, const String& mac, const String& ipAdress) :
		_bridgeId(bridgeId),
		_mac(mac),
		_iPAdress ( ipAdress)
	{
	}

	String ToOutput () override 
	{
		String result;
		StaticJsonBuffer<500> jsonBuffer;

		JsonObject& root = jsonBuffer.createObject();
		root["name"] = "hue emulator";
		root["swversion"] = "81012917";
		root["bridgeid"] = _bridgeId;
		root["portalservices"] = (bool)false;

		root["linkbutton"] = (bool)false;
		root["mac"] = _mac;
		root["dhcp"] = (bool)true;
		root["ipaddress"] = _iPAdress;
		root["netmask"] = "255.255.255.0";
		root["gateway"] = "192.168.1.1";
		root["apiversion"] = "1.3.0";
		root["localtime"] = "2017-12-03T12:07:32";
		root["timezone"] = "Europe/London";

		root["whitelist"] = RawJson("{\"api\":{\"name\":\"clientname#devicename\"}}");
		root["swupdate"] = RawJson("{\"text\":\"\", \"notify\" : false, \"updatestate\" : 0, \"url\" : \"\"}");

		root.printTo(result);
		
		return result;
	}


};