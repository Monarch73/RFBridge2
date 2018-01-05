// WcFnRequestHandler.h
#pragma once
#ifndef _WCFNREQUESTHANDLER_h
#define _WCFNREQUESTHANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ESP8266WebServer.h>

class WcFnRequestHandler : public RequestHandler
{

public:
	typedef std::function<void(WcFnRequestHandler *handler, String requestUri, HTTPMethod method)> HandlerFunction;
	WcFnRequestHandler(HandlerFunction&, const String&, HTTPMethod&, char = '*');
	bool canHandle(HTTPMethod, String) override;
	bool canUpload(String) override;
	bool handle(ESP8266WebServer&, HTTPMethod, String) override;
	void upload(ESP8266WebServer&, String, HTTPUpload&) override;
	String getWildCard(int);

private:
	String getPathSegment(String);
	String removeSlashes(String);
	String removePathSegment(String);
	String getWildCard(String, String, int, char = '*');

protected:
	String currentReqUri;
	HandlerFunction _fn;
	String _uri;
	HTTPMethod _method;
	char _wildcard;

};



#endif

