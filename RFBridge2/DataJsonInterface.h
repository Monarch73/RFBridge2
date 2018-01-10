#pragma once

#include <ArduinoJson.h>

class DataJsonInterface
{
public:
	virtual String ToOutput() { return ""; }
};
