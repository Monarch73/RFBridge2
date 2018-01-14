#pragma once
#ifndef REMOTECONTROL_H

#include "DipSwitches.h"
#include <RCSwitch.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

typedef struct dipswitches_struct dipswitch;

class RemoteControl
{
public:
	static RCSwitch *_Switch;
	static IRsend* _myIr;

	static void Send(dipswitch* dp, bool onoff)
	{
		dipswitch measure;
		if (onoff == false)
		{
			Serial.print("Turn Off ");
			if (strlen(dp->tri2) > 2)
			{
				Serial.println("tristate");
				RemoteControl::_Switch->sendTriState(dp->tri2);
			}
			else if (dp->irhz != 0)
			{
				for (int i = 0; i <(int)(sizeof(measure.irDataOff) / sizeof(uint16_t)); i++)
				{
					Serial.println("ir");
					if (dp->irDataOff[i] == 0xc1a0)
					{
						Serial.println("Sending bytes count: " + String(i - 1));
						_myIr->sendRaw(dp->irDataOff, i - 1, dp->irhz);
						break;
					}
				}
			}
			else   if (strlen(dp->urlOff) > 2)
			{
				Serial.println("url");
				WebInterface::SetUrlToCall(strdup(dp->urlOff));
			}
			else 
			{
				Serial.println("house/device code");
				RemoteControl::_Switch->switchOff(dp->housecode, dp->code);
			}
			
		}
		else
		{
			Serial.print("Turn On ");
			if (strlen(dp->tri1) > 2)
			{
				Serial.println("tristate");
				RemoteControl::_Switch->sendTriState(dp->tri1);
			}
			else if (dp->irhz != 0)
			{
				Serial.println("ir");
				for (int i = 0; i <(int)(sizeof(measure.irDataOn) / sizeof(uint16_t)); i++)
				{
					if (dp->irDataOn[i] == 0xc1a0)
					{
						Serial.println("Sending bytes count: " + String(i - 1));
						_myIr->sendRaw(dp->irDataOn, i - 1, dp->irhz);
						break;
					}
				}
			}
			else if (strlen(dp->urlOn) > 2)
			{
				Serial.println("url");
				WebInterface::SetUrlToCall(strdup(dp->urlOn));
			}
			else
			{
				Serial.println("house/device code");
				RemoteControl::_Switch->switchOn(dp->housecode, dp->code);
			}
		}

	}
};

RCSwitch *RemoteControl::_Switch;
IRsend *RemoteControl::_myIr;

#endif // REMOTECONTROL_H
