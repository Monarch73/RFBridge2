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
		WebInterface::SetDevices(&mySwitch, server);
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
		WebInterface::SetDevices(&mySwitch, server);
		server->on("/", HTTP_GET, WebInterface::HandleSetupRoot);
		server->on("/setup", HTTP_POST, WebInterface::handleSetupSSID);
		server->begin();

		return;
	}

	if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
		Serial.println("Error setting up MDNS responder!");
	}

	server = new ESP8266WebServer(80);
	WebInterface::SetDevices(&mySwitch, server);
	server->on("/", HTTP_GET, WebInterface::HandleRoot);
	on(WebInterface::ConfigFn, "/api/*/config", HTTP_ANY);

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
	Serial.println("Ready");

	otaEnabled = true;


}

// the loop function runs over and over again until power down or reset
void loop() {
	if (otaEnabled)
	{
		ArduinoOTA.handle();
	}
}
