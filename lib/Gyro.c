
#include "hitechnic-gyro.h"

typedef struct Gyro;
void initializeGyro(Gyro &gyro, tSensors sensor);
void updateGyro(Gyro &gyro);
void resetGyro(Gyro  &gyro);

typedef struct {
	float currentHeading;
	float offset;
	float rotationalSpeed;
	tSensors sensor;
	int timeIntervals;
	int initialTime;
} Gyro;

void initializeGyro(Gyro &gyro, tSensors sensor) {
	gyro.sensor = sensor;
	HTGYROstartCal(gyro.sensor);
	gyro.initialTime = nSysTime;
	gyro.timeIntervals = 0;
	time1[T1] = 0;
	time1[T2] = 0;
}

void resetGyroTimer(Gyro &gyro) {
	time1[T1] = 0;
}

void resetGyro(Gyro &gyro) {
	gyro.currentHeading = 0;
}

void updateGyro(Gyro &gyro) {

	// Read the rotational speed of the gyro
	gyro.rotationalSpeed = HTGYROreadRot(gyro.sensor);

	// Get the rotational speed
	gyro.currentHeading += gyro.rotationalSpeed * time1[T1] / 1000;

	time1[T1] = 0;

	if (time1[T2] > 100000) {
		//gyro.currentHeading += 0.0083333333333333333333333;
		//time1[T2] = 0;
	}
}

float getHeading(Gyro &gyro) {
	return gyro.currentHeading;
}
