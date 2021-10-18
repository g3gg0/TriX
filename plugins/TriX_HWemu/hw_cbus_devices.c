#ifndef __TRIX_HW_CBUS_DEVICES_C__
#define __TRIX_HW_CBUS_DEVICES_C__



#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "options.h"

#include "mem.h"


#include "trixplug.h"

#include "hwemu.h"

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

struct trix_functions *ft;

extern int verbosity;

extern int uem_pwr_key_pressed;
extern int uem_charger_connected;
extern int uem_adc_values[13];

int uem_adc_chan = 0;
int uem_adc_value = 0;

/*
     1  - battery temperature, random source
     2  - random source
     5  - headset
     6  - BSI
     9  - charger voltage
     10 - charger current
     11 - random source
     12 - VBat                              faked to 3.372V
     13 - hook state
*/

unsigned int hwemu_cbus_handle ( t_workspace *ws, unsigned char type, unsigned int dev, unsigned int reg, unsigned int val, unsigned int len )
{
	int okay = 0;
	int read = 1;
	int write = 0;
	unsigned int data = 0;
	unsigned char *device = "Unknown Device";
	unsigned char *desc = "";

	if ( type & MODE_WRITE )
	{
		read = 0;
		write = 1;
	}

	switch ( dev )
	{
		case 0x01:
			device = "UEM";

			switch ( reg & 0x1F)
			{
				case 0x00:
					if ( read )
					{
						desc = "ASIC Version";
						data = 0x0190;
					}
					break;

				case 0x01:
					desc = "RTC Alarm";
					break;

				case 0x02:
					desc = "RTC Alarm";
					break;

				case 0x03:
					desc = "RTC Alarm";
					break;

				case 0x04:
					desc = "RTC calibration value";
					break;

				case 0x09:
					desc = "AD Converter sample delay";
					break;

				case 0x0A:
					desc = "Audio Control";
					break;

				case 0x0B:
					desc = "Voltage Selection";
					break;

				case 0x0D:
				case 0x0E:
					desc = "Charger";
					break;

				case 0x0F:
					desc = "LCD PWM";
					break;

				case 0x11:
					if ( read )
						desc = "RTC Current Time Seconds";
					else
						desc = "Vibra PWM";
					break;

				case 0x12:
					if ( read )
						desc = "RTC Current Time Minutes";
					else
						desc = "Watchdog";
					break;

				case 0x13:
					if ( read )
						desc = "RTC Current Time Hours";
					else
						desc = "Watchdog";
					break;

				case 0x15:
				case 0x16:
					if ( write )
						desc = "Interrupts";
					break;

				case 0x17:
					if ( write )
						desc = "Audio Path";
					break;

				case 0x18:
					if ( write )
					{
						if ( val & 0x0400 )
							desc = "Buzzer Freq (enabled)";
						else
							desc = "Buzzer Freq (disabled)";
					}
					break;

				case 0x19:
					if ( write )
					{
						desc = "ADC Control";
						uem_adc_chan = val >> 12;
						if ( val & 0x0800 )
							printf ( "UEM A/D Conversion started for channel %i\n", uem_adc_chan );
						uem_adc_value = uem_adc_values[uem_adc_chan];
					}
					break;

				case 0x1A:
					if ( write )
						desc = "UEM WDT Password";
					else
					{
						desc = "ADC/MSIC Data";
						okay = 1;
						data = uem_adc_value;
					}
					break;

				case 0x1B:
					if ( write )
						desc = "Audio Path/Charger";
					else
					{
						desc = "UEM IMEI 0";
						data = 0x1C00;
					}
					break;

				case 0x1C:
					if ( write )
						desc = "Audio Path";
					else
					{
						desc = "UEM IMEI 1";
						data = 0x3365;
					}
					break;

				case 0x1D:
					if ( read )
					{
						desc = "UEM IMEI 2";
						data = 0x4601;
					}
					break;

				case 0x1E:
					if ( read )
					{
						desc = "UEM IMEI 3";
						data = 0x0004;
					}
					break;

				case 0x1F:
					if ( read )
					{
						desc = "Power key/Charger state";
						data = 0x0000;
						if ( !uem_pwr_key_pressed )
							data |= 0x0002;
						if ( uem_charger_connected )
							data |= 0x0008;
					}
					break;

			}
	}

	if ( verbosity & HWEMU_VERB_CBUS )
	{
		if ( type & MODE_WRITE )
			printf ( "cbus_write ( 0x%02X, 0x%02X, 0x%04X, %i ); // %s: %s\n", dev, reg, val, len, device, desc );
		else
			printf ( "cbus_read ( 0x%02X, 0x%02X, %i ); // %s: %s\n", dev, reg, len, device, desc );
	}
	return data;
}

#endif
