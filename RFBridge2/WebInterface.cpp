// 
// 
// 

#include "WebInterface.h"
#include <ArduinoJson.h>
#include "DataJsonConfig.h"
#include "DataJsonLight.h"
#include "DataJsonSuccess.h"
#include "DipSwitches.h"
#include "Progmem.h"
#include <pgmspace.h>
#include "RemoteControl.h"

EStore *WebInterface::estore;
RCSwitch *WebInterface::_mySwitch;
ESP8266WebServer* WebInterface::_myServer;
char * WebInterface::_hueId;
bool lightStates[N_DIPSWITCHES];
volatile char *WebInterface::_urlToCall;

void WebInterface::HandleDescription()
{
	String response;
	for (uint i = 0; i < strlen_P(description); i++)
	{
		char chr = pgm_read_byte(description + i);
		response += chr;
	}

	String mac = WiFi.macAddress();
	mac.toLowerCase();
	String udn = "uuid:2f402f80-da50-11e1-9b23-" + mac;

	response.replace("##URLBASE##", WiFi.localIP().toString());
	mac.toUpperCase();
	response.replace("##MAC##", mac);
	response.replace("##UDN##", udn);
	
	_myServer->send(200, "application/xml", response);
}

void WebInterface::SetUrlToCall(char * urlToCall)
{
	_urlToCall = urlToCall;
}

volatile char * WebInterface::GetUrlToCall() 
{
	return _urlToCall;
}

void WebInterface::HandleAngular(WcFnRequestHandler *handler, String requestUri, HTTPMethod method) 
{
	_myServer->sendHeader("Cache-Control", " max-age=120");
	_myServer->sendHeader("Last-Modified", "Wed, 21 Oct 2019 07:28:00 GMT");
	
	if (_myServer->hasHeader("If-Modified-Since"))
	{
		_myServer->send(304, "");
		return;
	}

	if (requestUri.startsWith("/styles"))
	{
		_myServer->send_P(200, "text/css",  ANGULAR_STYLES);
	}
	else if (requestUri.startsWith("/polyfill"))
	{
		_myServer->send_P(200, "text/javascript", ANGULAR_POLYFILLS);
	}
	else if (requestUri.startsWith("/inline"))
	{
		_myServer->send_P(200, "text/javascript", ANGULAR_INLINE);
	}
	else if (requestUri.startsWith("/main"))
	{
		_myServer->send_P(200, "text/javascript", ANGULAR_MAIN);
	}
	else
	{
		_myServer->send_P(200, "text/html", ANGULAR_INDEX);
	}
}

void WebInterface::WholeConfigFn(WcFnRequestHandler *handler, String requestUri, HTTPMethod method)
{
	if (requestUri == "/api/" && method == HTTP_POST)
	{
		DataJsonSuccess* success = new DataJsonSuccess("username","api");
		_myServer->send(200, "application/json", success->ToOutput());
	}
	else
	{

		String lightsList = GetLightsList();
		DataJsonConfig *jsonConfig = new DataJsonConfig(_hueId, WiFi.macAddress(), WiFi.localIP().toString());
		String config = jsonConfig->ToOutput();

		lightsList.remove(lightsList.length() - 1);
		if (lightsList.length() > 2)
		{
			lightsList += ",";
		}
		lightsList += "\"groups\": { },";
		lightsList += "\"config\": " + config + ", \"schedules\": { }, \"scenes\": { }, \"rules\": { }, \"sensors\": { }, \"resoucelinks\": { } }";

		_myServer->send(200, "application/json", lightsList);
	}
}

void WebInterface::ConfigFn(WcFnRequestHandler *handler, String requestUri, HTTPMethod method) {
	Serial.println("Reply by Config");
	switch (method) {
	case HTTP_GET: {
		DataJsonConfig *jsonConfig = new DataJsonConfig(_hueId, WiFi.macAddress(), WiFi.localIP().toString());
		SendJson(jsonConfig);
		break;
	}
	case HTTP_PUT: {
		// TODO: actually store this
		break;
	}
	default:
		break;
	}
}

String WebInterface::GetLightsList()
{
	typedef struct dipswitches_struct dipswitch;
	dipswitch dp;

	String responseContent = "{";
	int currentLight = 1;
	for (int i = 0; i < N_DIPSWITCHES; i++)
	{
		WebInterface::estore->dipSwitchLoad(i, &dp);
		if (dp.name[0] != 0)
		{
			if (currentLight != 1)
			{
				responseContent += ",";
			}

			DataJsonLight *objLight = new DataJsonLight(i + 1, dp.name, lightStates[i]);
			String light = objLight->ToOutput();
			responseContent += "\"";
			responseContent += String(currentLight);
			responseContent += "\":";
			responseContent += light;
			currentLight++;
		}
	}

	responseContent += "}";
	return responseContent;
}
void WebInterface::LightsFn(WcFnRequestHandler *handler, String requestUri, HTTPMethod method) 
{
	Serial.println("Reply by LightsList");
	_myServer->send(200, "application/json", GetLightsList());
}

void WebInterface::LightControlFn(WcFnRequestHandler *handler, String requestUri, HTTPMethod method) 
{
	Serial.println("Reply by Lightscontrol");

	typedef struct dipswitches_struct dipswitch;
	dipswitch dp;
	int numberOfTheLight = atoi(handler->getWildCard(1).c_str()) - 1;

	Serial.println(numberOfTheLight);
	int currentLight = 0;
	String onJson = _myServer->arg("plain");
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(onJson);
	if (root.success())
	{
		bool on = root["on"];
		Serial.println(on);
		for (int i = 0; i < N_DIPSWITCHES; i++)
		{
			WebInterface::estore->dipSwitchLoad(i, &dp);
			if (dp.name[0] != 0)
			{
				if (currentLight == numberOfTheLight)
				{

					if (on == true)
					{
						Serial.println("turning light on");
						RemoteControl::Send(&dp, true);
						lightStates[i] = true;
					}
					else
					{
						Serial.println("turning light off");
						RemoteControl::Send(&dp, false);
						lightStates[i] = false;
					}
				}

				currentLight++;
			}
		}
	}
	WebInterface::LightFn(handler, requestUri, method);
}

void WebInterface::LightFn(WcFnRequestHandler *handler, String requestUri, HTTPMethod method) 
{
	Serial.println("Reply by Light");

	typedef struct dipswitches_struct dipswitch;
	dipswitch dp;
	int numberOfTheLight = atoi(handler->getWildCard(1).c_str()) - 1;
	int currentLight = 0;
	String response;
	for (int i = 0; i < N_DIPSWITCHES; i++)
	{
		WebInterface::estore->dipSwitchLoad(i, &dp);
		if (dp.name[0] != 0)
		{
			if (currentLight == numberOfTheLight)
			{
				DataJsonLight *objLight = new DataJsonLight(i+1, dp.name, lightStates[i]);
				response = objLight->ToOutput();
			}
			currentLight++;
		}
	}
	_myServer->send(200, "application/json", response);
}


void WebInterface::HandleJsonList()
{
	typedef struct dipswitches_struct dipswitch;
	dipswitch dp;
	char *responseContent = (char*)malloc(1024);
	if (responseContent == NULL)
	{
		Serial.println("out of memory");
		return;
	}

	strcpy(responseContent, "[ ");

	bool entry = false;
	for (int i = 0; i < N_DIPSWITCHES; i++)
	{
		WebInterface::estore->dipSwitchLoad(i, &dp);
		if (dp.name[0] != 0)
		{
			char buf[49];
			entry = true;
			sprintf(buf, " { \"i\": %d, \"n\": \"%s\" },", i, dp.name);
			strcat(responseContent, buf);
		}
	}
	if (entry)
	{
		responseContent[strlen(responseContent) - 1] = 0;
	}

	strcat(responseContent, " ]");
	_myServer->sendHeader("Access-Control-Allow-Origin", "*");
	_myServer->send(200, "application/json", responseContent);
	free(responseContent);
}

void WebInterface::HandleEDelete()
{
	String a = _myServer->arg("no");
	if (a != "")
	{
		int no = atoi(a.c_str());
		if (no < N_DIPSWITCHES)
		{
			Serial.println(no);
			estore->dipSwitchDelete(no);
		}
	}
	_myServer->send(200, "");
}


void WebInterface::HandleEStore()
{
	typedef struct dipswitches_struct dipswitch;
	dipswitch dp;
	String a = _myServer->arg("name");
	String b = _myServer->arg("house");
	String c = _myServer->arg("code");
	String c2 = _myServer->arg("tri1");
	String c3 = _myServer->arg("tri2");
	String d1 = _myServer->arg("url1");
	String d2 = _myServer->arg("url2");
	Serial.println("storing");
	Serial.println(a);
	Serial.println(b);
	Serial.println(c);
	Serial.println(c2);
	Serial.println(c3);
	Serial.println(d1);
	Serial.println(d2);

	int no = estore->dipSwitchFindFree();
	if (no >= 0 && _myServer->method() == HTTP_POST)
	{
		memcpy(dp.name, (char *)a.c_str(), sizeof(dp.name));
		memcpy(dp.housecode, (char *)b.c_str(), sizeof(dp.housecode));
		memcpy(dp.code, (char *)c.c_str(), sizeof(dp.code));
		memcpy(dp.tri1, (char*)c2.c_str(), sizeof(dp.tri1));
		memcpy(dp.tri2, (char *)c3.c_str(), sizeof(dp.tri2));
		memcpy(dp.urlOn, (char *)d1.c_str(), sizeof(dp.urlOn));
		memcpy(dp.urlOff, (char *)d2.c_str(), sizeof(dp.urlOff));
		dp.name[sizeof(dp.name) - 1] = 0;
		dp.housecode[sizeof(dp.housecode) - 1] = 0;
		dp.code[sizeof(dp.code) - 1] = 0;
		dp.tri1[sizeof(dp.tri1) - 1] = 0;
		dp.tri2[sizeof(dp.tri2) - 1] = 0;
		dp.urlOff[sizeof(dp.urlOff) - 1] = 0;
		dp.urlOn[sizeof(dp.urlOn) - 1] = 0;
		estore->dipSwitchSave(no, &dp);
	}

	_myServer->send(200, "");

}

void WebInterface::HandleSetupRoot()
{
	char setupoutputbuffer[sizeof(HTML_HEADER_SETUP) + sizeof(HTML_SSID) + 5];
	strcpy_P(setupoutputbuffer, HTML_HEADER_SETUP);
	strcat_P(setupoutputbuffer, HTML_SSID);
	_myServer->send(200, "text/html", setupoutputbuffer);
}

void WebInterface::handleSetupSSID()
{
	char setupoutputbuffer[sizeof(HTML_HEADER_SETUP) + sizeof(HTML_SSIDOK) + 5];
	String a = _myServer->arg("ssid");
	String b = _myServer->arg("password");
	String c = _myServer->arg("format");

	bool format = false;
	if (c.compareTo("format") == 0)
	{
		format = true;
	}

	strcpy(WebInterface::estore->ssid, a.c_str());
	strcpy(WebInterface::estore->password, b.c_str());

	WebInterface::estore->wifiSave(format);

	strcpy_P(setupoutputbuffer, HTML_HEADER_SETUP);
	strcat_P(setupoutputbuffer, HTML_SSIDOK);
	_myServer->send(200, "text/html", setupoutputbuffer);
}

void WebInterface::HandleFormat()
{
	WebInterface::estore->wifiSave(true);
	WebInterface::HandleSetupRoot();
}

void WebInterface::HandleRoot()
{

}

void WebInterface::SetDevices(RCSwitch *rc, ESP8266WebServer *server, const char *hueId)
{
	_mySwitch = rc;
	_myServer = server;
	_hueId = strdup(hueId);
	RemoteControl::_Switch = rc;
}


void WebInterface::SendJson(DataJsonInterface *json)
{
	String response = json->ToOutput();
	_myServer->sendHeader("Access-Control-Allow-Origin", "*");
	_myServer->send(200, "application/json", response);
}