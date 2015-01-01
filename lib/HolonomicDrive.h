#define fourWheelDrive = 4;
#define threeWheelDrive = 3;

#define JOYINPUT = 0;
#define MANUAL = 1;

#define DEGREE_90 = 90;
#define DEGREE_180 = 180;
#define DEGREE_270 = 270;


#include "SmoothMoves.h"
#include "JoystickDriver.c"

typedef struct MotorSetup;
typedef struct MotorDriveType;
typedef struct InputMethod;
typedef struct Holonomic;
void moveRobot(int directionInDegrees, int speed, float distance);
void setRobotDirection(int directionInDegrees, float speed);
void sendRobot(float directionInDegrees, float speed);
int speedFromJoystick(TJoystick &joystick);
float map(float value, float a, float b, float c, float d);

typedef struct {
	int motorFrontLeftID;
	int motorFrontRightID;
	int motorBackLeftID;
	int motorBackRightID;
} MotorSetup;

typedef struct {
	bool useIntergratedPID;
} Holonomic;

typedef struct {
	int type;
} MotorDriveType;

typedef struct {
	int method;
} InputMethod;

void setUpMotors(MotorSetup &motorSetup, const int motorFrontLeftID, const int motorFrontRightID, const int motorBackLeftID, const int motorBackRightID) {
	motorSetup.motorFrontLeftID = motorFrontLeftID;
	motorSetup.motorFrontRightID = motorFrontRightID;
	motorSetup.motorBackLeftID = motorBackLeftID;
	motorSetup.motorBackRightID = motorBackRightID;
}

void setRobotDirection(int directionInDegrees, float speed) {

}

// Updates the holonimc drive
void updateHolonomicDrive(int inputMethod) {

}

// Decodes the joystick input to get the angle
float directionFromJoystick(TJoystick &joystick) {

	int section = 0;

	float degree;

	// gets degree ranging from 45 to 90 degrees
	if (abs(joystick.joy1_x1) > abs(joystick.joy1_y1)) {
		degree = radiansToDegrees(atan(abs(joystick.joy1_x1 / joystick.joy1_y1)));
		} else {
		degree = radiansToDegrees(atan(abs(joystick.joy1_y1 / joystick.joy1_x1)));
	}

	if (joystick.joy1_x1 < 0 && joystick.joy1_y1 < 0) {
		// Sector 0 calculations
		section = 0;
		if (abs(joystick.joy1_x1) < abs(joystick.joy1_y1)) {
			degree = 90 - degree;
		}
		degree += 180;
		} else if (joystick.joy1_x1 > 0 && joystick.joy1_y1 < 0) {
		// Sector 1 calculations
		section = 1;
		if (abs(joystick.joy1_x1) > abs(joystick.joy1_y1)) {
			degree = 90 - degree;
		}
		degree += 90;
		} else if (joystick.joy1_x1 > 0 && joystick.joy1_y1 > 0) {
		// Sector 2 calculations
		section = 2;
		if (abs(joystick.joy1_x1) < abs(joystick.joy1_y1)) {
			degree = 90 - degree;
		}
		degree += 0;
		} else if (joystick.joy1_x1 < 0 && joystick.joy1_y1 > 0) {
		// Sector 3 calculations
		section = 3;
		if (abs(joystick.joy1_x1) > abs(joystick.joy1_y1)) {
			degree = 90 - degree;
		}
		degree += 270;
	}

		if (abs(joystick.joy1_x1) < 5 && abs(joystick.joy1_y1) < 5) {
		degree = -1;
		return degree;
	}

	return degree;
}

float speedFromJoystick(TJoystick &joystick) {

	float speed;

	if (abs(joystick.joy1_x1) > abs(joystick.joy1_y1)) {
		speed = abs(joystick.joy1_x1);
	} else {
		speed = abs(joystick.joy1_y1);
	}

	return map(speed, 0, 128, 0, 100);
}

float map(float value, float a, float b, float c, float d) {
	value -= a;
	b -= a;
	d -= c;
	float returnValue = value / b * d + c;
	return returnValue;
}
