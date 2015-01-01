#include "hitechnic-irseeker-v2.h"

typedef struct IRSeekerInfo;

typedef struct {
	// the default DSP mode is 1200 Hz.
	tHTIRS2DSPMode mode;

	//Sensor Port
	tSensors sensorPort;

	//Direction Reading
	int dir_ac, dir_dc;

	//sensor values
	int dcS1, dcS2, dcS3, dcS4, dcS5;
	int acS1, acS2, acS3, acS4, acS5;

	//extra values
	int dirEnh, strEnh;
} IRSeekerInfo;

bool initialieIRSeeker(IRSeekerInfo &irseekerInfo, tSensors sensorPort) {
	irseekerInfo.mode = DSP_1200;

	irseekerInfo.sensorPort = sensorPort;

	int x = 0;

	// wait until sensor is initialized
	while (x > 1000) {
		if (HTIRS2setDSPMode(sensorPort, irseekerInfo.mode)) {
			break;
		}
		wait1Msec(1);
		x++;
	}

	if (x == 1000)
		return false;

	return true;
}

bool updateIRSeeker(IRSeekerInfo &irseekerInfo) {
	// Read the current non modulated signal direction
	irseekerInfo.dir_dc = HTIRS2readDCDir(irseekerInfo.sensorPort);
	if (irseekerInfo.dir_dc < 0)
		return false; // I2C read error occurred

	////// read the current modulated signal direction
	irseekerInfo.dir_ac = HTIRS2readACDir(irseekerInfo.sensorPort);
	if (irseekerInfo.dir_ac < 0)
		return false; // I2C read error occurred

	///// Read the individual signal strengths of the internal sensors
	//// Do this for both unmodulated (DC) and modulated signals (AC)
	if (!HTIRS2readAllDCStrength(irseekerInfo.sensorPort, irseekerInfo.dcS1, irseekerInfo.dcS2, irseekerInfo.dcS3, irseekerInfo.dcS4, irseekerInfo.dcS5))
		return false; // I2C read error occurred
	if (!HTIRS2readAllACStrength(irseekerInfo.sensorPort, irseekerInfo.acS1, irseekerInfo.acS2, irseekerInfo.acS3, irseekerInfo.acS4, irseekerInfo.acS5))
		return false; // I2C read error occurred

	////// Read the Enhanced direction and strength
	if (!HTIRS2readEnhanced(irseekerInfo.sensorPort, irseekerInfo.dirEnh, irseekerInfo.strEnh))
		return false; // I2C read error occurred

	return true;


}
