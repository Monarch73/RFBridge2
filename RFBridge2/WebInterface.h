// WebInterface.h

#ifndef _WEBINTERFACE_h
#define _WEBINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ESP8266WebServer.h>
#include <RCSwitch.h>
#include "EStore.h"
#include "WcFnRequestHandler.h"
#include "DataJsonInterface.h"

class WebInterface
{
public:
	static EStore *estore;
	static void HandleSetupRoot();
	static void HandleFormat();
	static void handleSetupSSID();
	static void HandleRoot();
	static void SetDevices(RCSwitch *mySwitch, ESP8266WebServer *myServer);
	static void ConfigFn(WcFnRequestHandler *handler, String requestUri, HTTPMethod method);

private:
	static RCSwitch* _mySwitch;
	static ESP8266WebServer* _myServer;
	static void SendJson(DataJsonInterface* data);
};

#endif
