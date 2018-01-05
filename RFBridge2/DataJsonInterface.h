#pragma once

#include <ArduinoJson.h>

class DataJsonInterface
{
public:
	virtual void ToOutput(char*buffer, size_t len) { return; }
};
