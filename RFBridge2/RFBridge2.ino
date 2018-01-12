/*

 Name:		RFBridge2.ino
 Created:	29.12.2017 16:03:45
 Author:	niels
*/

// the setup function runs once when you press reset or power the board
#include "WcFnRequestHandler.h"
#include "WebInterface.h"
#include "EStore.h"
#include "SSDP.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <RCSwitch.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <assert.h>
#include <ESP8266HTTPClient.h>


ESP8266WebServer* server;
RCSwitch mySwitch = RCSwitch();
bool otaEnabled = false;

typedef std::function<void(WcFnRequestHandler *, String, HTTPMethod)> HandlerFunction;

void on(HandlerFunction fn, const String &wcUri, HTTPMethod method, char wildcard = '*') {
	server->addHandler(new WcFnRequestHandler(fn, wcUri, method, wildcard));
}


void setup() {
	Serial.begin(115200);
	mySwitch.enableTransmit(2);
	int zahl = analogRead(A0);
	Serial.println(zahl);
	String hueId = WiFi.macAddress();
	hueId.replace(":", "");
	hueId = hueId.substring(0, 6) + "FFFE" + hueId.substring(6);


	WebInterface::estore = new EStore(); // &estore2;
	if (zahl > 350)
	{
		WebInterface::estore->setupEeprom(true);
	}
	else
	{
		WebInterface::estore->setupEeprom();
	}

	if (WebInterface::estore->ssid[0] == 0)
	{
		Serial.println("No ssid defined");
		/* You can remove the password parameter if you want the AP to be open. */
		WiFi.softAP("EasyAlexa");

		IPAddress myIP = WiFi.softAPIP();
		Serial.print("AP IP address: ");
		Serial.println(myIP);
		server = new ESP8266WebServer(80);
		WebInterface::SetDevices(&mySwitch, server, hueId.c_str());
		server->on("/", HTTP_GET, WebInterface::HandleSetupRoot);
		server->on("/setup", HTTP_POST, WebInterface::handleSetupSSID);
		server->begin();
		return;
	}

	WiFi.mode(WIFI_STA);
	WiFi.begin(WebInterface::estore->ssid, WebInterface::estore->password);
	zahl = 0;
	if (WiFi.waitForConnectResult() != WL_CONNECTED)
	{
		Serial.println("WiFi Failed.Entering setup mode.");
		Serial.println("No ssid defined");
		/* You can remove the password parameter if you want the AP to be open. */
		WiFi.softAP("EasyAlexa");

		IPAddress myIP = WiFi.softAPIP();
		Serial.print("AP IP address: ");
		Serial.println(myIP);
		server = new ESP8266WebServer(80);
		WebInterface::SetDevices(&mySwitch, server,hueId.c_str());
		server->on("/", HTTP_GET, WebInterface::HandleSetupRoot);
		server->on("/setup", HTTP_POST, WebInterface::handleSetupSSID);
		server->begin();

		return;
	}

	if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
		Serial.println("Error setting up MDNS responder!");
	}

	server = new ESP8266WebServer(80);
	WebInterface::SetDevices(&mySwitch, server, hueId.c_str());
	on(WebInterface::ConfigFn, "/api/*/config", HTTP_ANY);
	on(WebInterface::LightFn, "/api/*/lights/*", HTTP_ANY);
	on(WebInterface::LightControlFn, "/api/*/lights/*/state", HTTP_ANY);
	on(WebInterface::LightsFn, "/api/*/lights", HTTP_ANY);
	on(WebInterface::WholeConfigFn, "/api", HTTP_ANY);
	on(WebInterface::WholeConfigFn, "/api/", HTTP_ANY);
	on(WebInterface::ConfigFn, "/api/api", HTTP_ANY);
	on(WebInterface::HandleAngular,"/", HTTP_ANY);
	on(WebInterface::HandleAngular, "styles.89c7d201f868ab33b8ed.bundle.css", HTTP_ANY);
	on(WebInterface::HandleAngular, "inline.6022114626152249fbb3.bundle.js", HTTP_ANY);
	on(WebInterface::HandleAngular, "polyfills.5b59249e2a37b3779465.bundle.js", HTTP_ANY);
	on(WebInterface::HandleAngular, "main.e8c6b586049960613364.bundle.js", HTTP_ANY);
	server->on("/description.xml", HTTP_GET, WebInterface::HandleDescription);
	server->on("/jsonList", HTTP_GET, WebInterface::HandleJsonList );
	server->on("/estore", HTTP_POST , WebInterface::HandleEStore);
	server->on("/edelete", HTTP_GET, WebInterface::HandleEDelete);

	const char * headerkeys[] = { "User-Agent","Cookie", "If-Modified-Since" };
	size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
	//ask server to track these headers
	server->collectHeaders(headerkeys, headerkeyssize);

	server->begin();
	ArduinoOTA.onStart([]() {
		Serial.println("Start updating spiffs");
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		__asm__("nop\n\t");
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});
	ArduinoOTA.begin();

	Serial.println("Hue Emulation startet: " + WiFi.localIP().toString());

	otaEnabled = true;
	
	String serial = WiFi.macAddress();
	serial.toLowerCase();
	Serial.println("Starting SSDP...");
	SSDP.setSchemaURL("description.xml");
	SSDP.setHTTPPort(80);
	SSDP.setName("Philips hue clone");
	SSDP.setSerialNumber(serial.c_str());
	SSDP.setURL("index.html");
	SSDP.setModelName("IpBridge");
	SSDP.setModelNumber("0.1");
	SSDP.setModelURL("http://www.meethue.com");
	SSDP.setManufacturer("Royal Philips Electronics");
	SSDP.setManufacturerURL("http://www.philips.com");
	SSDP.setIpAdress(WiFi.localIP().toString().c_str());
	SSDP.setBridgeId(hueId.c_str());

	//SSDP.setDeviceType((char*)"upnp:rootdevice");
	SSDP.setDeviceType("urn:schemas-upnp-org:device:basic:1");
	//SSDP.setMessageFormatCallback(ssdpMsgFormatCallback);
	SSDP.begin();
	Serial.println("SSDP Started");

}

// the loop function runs over and over again until power down or reset
void loop() {
	volatile char *urlToCall2;

	if (otaEnabled)
	{
		ArduinoOTA.handle();
	}
	server->handleClient();

	if ((urlToCall2 = WebInterface::GetUrlToCall()) != NULL)
	{
		Serial.print("Calling ");
		Serial.print((char *)urlToCall2);
		HTTPClient http;
		bool ret = http.begin((char *)urlToCall2);
		Serial.println(ret);
		int ret2 = http.GET();
		Serial.print(" ");
		Serial.println(ret2);
		http.end();
		WebInterface::SetUrlToCall(NULL);
		free((void *)urlToCall2);
	}
}
