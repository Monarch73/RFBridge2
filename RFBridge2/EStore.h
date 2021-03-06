// EStore.h

#ifndef _ESTORE_h
#define _ESTORE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "DipSwitches.h"
#define N_DIPSWITCHES 50
#define N_CHAR_SSID   36
#define N_CHAR_PASSWORD 80
class EStore
{
public:
	static char ssid[N_CHAR_SSID];
	static char password[N_CHAR_PASSWORD];
	EStore();
	~EStore();
	void setupEeprom(bool = false);
	void dipSwitchLoad(int, dipswitches_struct *);
	void dipSwitchDelete(int);
	void dipSwitchSave(int, dipswitches_struct *);
	void wifiSave(bool format);
	int dipSwitchFindFree(void);
private:

};


#endif

