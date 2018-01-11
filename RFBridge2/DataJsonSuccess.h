#pragma once
#ifndef DATAJSONSUCCESS_H

#include "DataJsonInterface.h"
#include <ArduinoJson.h>

// [{"success":{"username":"api"}}]

class DataJsonSuccess : public DataJsonInterface
{
private:
	String _id;
	String _value;

public:
	DataJsonSuccess(const String& id, const String& value) :
		_id(id),
		_value(value)
	{

	}

	String ToOutput() override
	{
		String result;
		StaticJsonBuffer<200> json;
		auto& root = json.createArray();
		auto& innerRoot = root.createNestedObject();
		auto& successObject = innerRoot.createNestedObject("success");
		successObject[this->_id] = this->_value;
		root.printTo(result);
		return result;
	}
};

#endif