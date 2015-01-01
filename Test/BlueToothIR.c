#pragma config(Sensor, S3,     sensorIR,       sensorI2CCustom)

#include "irSeeker_lib.h"

task main()
{

	IRSeekerInfo irSeeker;
	initialieIRSeeker(irSeeker, sensorIR);

	while(true) {
		updateIRSeeker(irSeeker);
		//sendMessage(irSeeker.dir_ac + 100);
		//wait1Msec(100);

		hogCPU();
		sendMessage(irSeeker.dir_ac);
		wait1Msec(100);
		releaseCPU();

		nxtDisplayTextLine(1,"%4f", irSeeker.acS1);
		nxtDisplayTextLine(2,"%4f", irSeeker.acS2);
		nxtDisplayTextLine(3,"%4f", irSeeker.acS3);
		nxtDisplayTextLine(4,"%4f", irSeeker.acS4);
		nxtDisplayTextLine(5,"%4f", irSeeker.acS5);
	}
}
