#pragma once

#include <ArduinoJson.h>
#include "DataJsonInterface.h"
//{
//	"manufacturername":"OpenSource", 
//	"modelid" : "LST001", 
//	"name" : "Test 1", 
//	"state" : 
//	{
//		"on":##STATE##,
//		"hue" : 0,
//		"bri" : 0, 
//		"sat" : 0,
//		"xy" : [0.00000, 0.00000],
//		"ct" : 500,
//		"alert" : "none",
//		"effect" : "none",
//		"colormode" : "hs",
//		"reachable" : true
//	},
//	"swversion" : "0.1",
//	"type" : "Dimmable light",
//	"uniqueid" : "60:01:94:0E:9E:F7-1"
//}

class DataJsonLight : public DataJsonInterface
{
private:
	int _lightId;
	char *_name;
	bool _state;

public:
	DataJsonLight(int lightId, char *name, bool state ) :
		_lightId(lightId),
		_name(name),
		_state(state)
	{

	}

	String ToOutput() override
	{
		String retunValue;
		StaticJsonBuffer<400> jsonBuffer;
		JsonObject& root = jsonBuffer.createObject();
		String unique = WiFi.macAddress();
		unique.toUpperCase();
		unique += "-" + String(_lightId);
		root["type"] = "Dimmable light";
		root["manufacturername"] = "OpenSource";
		root["swversion"] = "0.1";
		root["name"] = _name;
		root["uniqueid"] = unique;
		root["modelid"] = "LST001";
		JsonObject& state = root.createNestedObject("state");
		state["on"] = _state;
		state["bri"] = 0;
		state["alert"] = "none";
		state["reachable"] = (bool)true;
		root.printTo(retunValue);
		return retunValue;
	}
};
