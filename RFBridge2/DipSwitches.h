#pragma once

#ifndef _DIPSWITCHES_h
#define _DIPSWITCHES_h
struct dipswitches_struct
{
	char name[20];
	char housecode[6];
	char code[6];
	char tri1[16];
	char tri2[16];
	char urlOn[160];
	char urlOff[160];
	uint16_t irhz;
	uint16_t irDataOn[100];
	uint16_t irDataOff[100];
};

#endif