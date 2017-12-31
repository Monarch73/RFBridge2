/*

 Name:		RFBridge2.ino
 Created:	29.12.2017 16:03:45
 Author:	niels
*/

// the setup function runs once when you press reset or power the board
#include "WebInterface.h"
#include "EStore.h"
#include "SSDP.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <RCSwitch.h>

ESP8266WebServer* server;
RCSwitch mySwitch = RCSwitch();

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

}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
