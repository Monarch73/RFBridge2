// 
// 
// 

#include "WebInterface.h"
#include <ArduinoJson.h>
#include "DataJsonConfig.h"

char HTML_HEADER[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><title>RFBridge</title><link rel=\"stylesheet/less\" type=\"text/css\" href=\"http://www.monarch.de/c64-theme/css/style.css\" />"
"<script src=\"http://www.monarch.de/c64-theme/js/less-1.3.0.min.js\" type=\"text/javascript\"></script><script>function changeFont(font) {document.getElementById('font-div').className = font; }"
"function changeFontSize(fontsize) { document.getElementById('font-size-div').className = fontsize; }</script><link rel=\"shortcut icon\" href=\"http://www.monarch.de/c64-theme/images/favicon.ico\">"
"</head><body><div class=\"container\"><div id=\"font-div\" class=\"c64u\">";

char HTML_HEADER_SETUP[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><title>RFBridge</title>"
"</head><body>";


char HTML_SSID[] PROGMEM =
"<h1>RFBridge SETUP</h1>"
"<p><form method=\"POST\" action=\"/setup\">SSID:<input type=\"TEXT\" name=\"ssid\" /> Password:<input type=\"TEXT\" name=\"password\" /><input type=\"checkbox\" id=\"format\" name=\"format\" value=\"format\" /><label for=\"format\">initialize (format) Storage</lable><input type=\"submit\" name=\"send\" value=\"save\" /></form></p>"
"</body>";

char HTML_SSIDOK[] PROGMEM =
"<h1>Configuration OK - Device restarting in a couple of seconds.</h1>"
"</body>";

EStore *WebInterface::estore;
RCSwitch *WebInterface::_mySwitch;
ESP8266WebServer* WebInterface::_myServer;

void WebInterface::HandleSetupRoot()
{
	char setupoutputbuffer[sizeof(HTML_HEADER_SETUP) + sizeof(HTML_SSID) + 5];
	strcpy_P(setupoutputbuffer, HTML_HEADER_SETUP);
	strcat_P(setupoutputbuffer, HTML_SSID);
	_myServer->send(200, "text / plain", setupoutputbuffer);
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
	_myServer->send(200, "text / plain", setupoutputbuffer);
}

void WebInterface::HandleFormat()
{
	WebInterface::estore->wifiSave(true);
	WebInterface::HandleSetupRoot();
}

void WebInterface::HandleRoot()
{

}

void WebInterface::SetDevices(RCSwitch *rc, ESP8266WebServer *server)
{
	_mySwitch = rc;
	_myServer = server;
}

void WebInterface::ConfigFn(WcFnRequestHandler *handler, String requestUri, HTTPMethod method) {
	switch (method) {
	case HTTP_GET: {
		DataJsonConfig *jsonConfig = new DataJsonConfig();
		jsonConfig->bridgeId = NULL;
		jsonConfig->iPAdress = (char *)WiFi.localIP().toString().c_str();
		jsonConfig->mac = (char *)WiFi.macAddress().c_str();
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

void WebInterface::SendJson(DataJsonInterface * data)
{
	char outputBuffer[1024];
	data->ToOutput((char*)&outputBuffer, 1024);
	_myServer->send(200, "application/json", outputBuffer);
}
