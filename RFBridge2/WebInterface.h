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
#include <IRremoteESP8266.h>
#include <IRsend.h>

class WebInterface
{
public:
	static EStore *estore;
	static void HandleSetupRoot();
	static void HandleFormat();
	static void handleSetupSSID();
	static void HandleAngular(WcFnRequestHandler *handler, String requestUri, HTTPMethod method);
	static void SetDevices(RCSwitch *mySwitch, ESP8266WebServer *myServer, const char *hueId, IRsend *ir);
	static void ConfigFn(WcFnRequestHandler *handler, String requestUri, HTTPMethod method);
	static void WholeConfigFn(WcFnRequestHandler *handler, String requestUri, HTTPMethod method);
	static void LightFn(WcFnRequestHandler *handler, String requestUri, HTTPMethod method);
	static void LightsFn(WcFnRequestHandler *handler, String requestUri, HTTPMethod method);
	static void LightControlFn(WcFnRequestHandler *handler, String requestUri, HTTPMethod method);
	static void HandleJsonList();
	static void HandleEStore();
	static void HandleEDelete();
	static void HandleDescription();
	static void HandleESocket();
	static void SetUrlToCall(char *urlToCall);
	volatile static char *GetUrlToCall();

private:
	volatile static char *_urlToCall;
	static RCSwitch* _mySwitch;
	static IRsend* _myIr;
	static ESP8266WebServer* _myServer;
	static char *_hueId;
	static void SendJson(DataJsonInterface *);
	static String GetLightsList();
	static void ParseStringNumbers(String& data, uint16_t*numbers);
};

#endif
