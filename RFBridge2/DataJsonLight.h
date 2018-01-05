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
public:
	char *uniqueID;
	char *name;
	bool state;

	void ToOutput(char*buffer, size_t len) override
	{
		StaticJsonBuffer<400> jsonBuffer;
		JsonObject& root = jsonBuffer.createObject();
		root["manufacturername"] = "OpenSource";
		root["modelid"] = "LST001";
		root["name"] = name;
		JsonObject& state = root.createNestedObject("state");
		state["on"] = state;
		state["hue"] = 0;
		state["bri"] = 0;
		state["sat"] = 0;
		state["xy"] = RawJson("[0.00000,0.00000]");
		state["ct"] = 500;
		state["alert"] = "none";
		state["effect"] = "none";
		state["colormode"] = "hs";
		state["reachable"] = (bool)true;
		root["swversion"] = "0.1";
		root["type"] = "Dimmable light";
		root["uniqueid"] = uniqueID;

		root.printTo(buffer, len);
		return;
	}
};
