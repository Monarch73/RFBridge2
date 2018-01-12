#pragma once
#ifndef REMOTECONTROL_H

#include "DipSwitches.h"
#include <RCSwitch.h>

typedef struct dipswitches_struct dipswitch;

class RemoteControl
{
public:
	static RCSwitch *_Switch;

	static void Send(dipswitch* dp, bool onoff)
	{
		if (onoff == false)
		{
			
			if (strlen(dp->tri2) > 2)
			{
				RemoteControl::_Switch->sendTriState(dp->tri2);
			}
			else   if (strlen(dp->urlOff) > 2)
			{
				Serial.print("Off ");
				Serial.println(dp->urlOff);
				WebInterface::SetUrlToCall(strdup(dp->urlOff));
			}
			else
			{
				RemoteControl::_Switch->switchOff(dp->housecode, dp->code);
			}
		}
		else
		{
			if (strlen(dp->tri1) > 2)
			{
				RemoteControl::_Switch->sendTriState(dp->tri1);
			}
			else if (strlen(dp->urlOn) > 2)
			{
				Serial.print("On ");
				Serial.println(dp->urlOn);
				WebInterface::SetUrlToCall(strdup(dp->urlOn));
			}
			else
			{
				RemoteControl::_Switch->switchOn(dp->housecode, dp->code);
			}
		}

	}
};

RCSwitch *RemoteControl::_Switch;

#endif // REMOTECONTROL_H
