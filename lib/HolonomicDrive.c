#include "HolonomicDrive.h"
#include "SmoothMoves.h"
#include "Gyro.c"

MotorDriveType motorDriveType;
MotorInfo motorFrontInfo;
MotorInfo motorRightInfo;
MotorInfo motorBackInfo;
MotorInfo motorLeftInfo;
Gyro gyro;
bool hasGyro;
int heading = 0;

// Initializes the Holonomic Drive controller MUST be called at robot initialization
void initializeHolonimicDrive(int motor0, int motor1, int motor2, int motor3) {
	motorFrontInfo.motorPort = motor0;
	motorRightInfo.motorPort = motor1;
	motorBackInfo.motorPort = motor2;
	motorLeftInfo.motorPort = motor3;

	motorFrontInfo.targetSpeed = 0;
	motorRightInfo.targetSpeed = 0;
	motorBackInfo.targetSpeed = 0;
	motorLeftInfo.targetSpeed = 0;

	motorFrontInfo.speedIncrement = 10;
	motorRightInfo.speedIncrement = 10;
	motorBackInfo.speedIncrement = 10;
	motorLeftInfo.speedIncrement = 10;
	motorDriveType.type = 4;

	hasGyro = false;
}

void addGyroControl(tSensors gyroPort) {
	initializeGyro(gyro, gyroPort);
	hasGyro = true;
}


//Sets the Motor Drive Type whether it is a four wheel based drive or three wheel
void setMotorDriveType(int type){
	motorDriveType.type = type;
}

// moves the robot two that place, use in Automonus mode ONLY!
void moveRobot(int directionInDegrees, int speed, float distance) {

}

// sets the robots soeed and direction
void sendRobot(float directionInDegrees, float speed) {

	if (hasGyro) {
		heading = gyro.currentHeading;
	}
	int dir = abs(heading - directionInDegrees);

	if (directionInDegrees == -1) {
		speed = 0;
	}

	setMotorTargetSpeed(motorFrontInfo, (speed * sinDegrees(directionInDegrees)));
	setMotorTargetSpeed(motorRightInfo, (speed * cosDegrees(directionInDegrees)));
	setMotorTargetSpeed(motorBackInfo, -(speed * sinDegrees(directionInDegrees)));
	setMotorTargetSpeed(motorLeftInfo, -(speed * cosDegrees(directionInDegrees)));
}

// Drive robot forward/back
void driveForward(int speed) {
	//setMotorTargetSpeed(motorFrontInfo, -speed * sin(0));
	setMotorTargetSpeed(motorRightInfo, -speed * cos(0));
	//setMotorTargetSpeed(motorBackInfo, speed * sin(0));
	setMotorTargetSpeed(motorLeftInfo, speed * cos(0));

	updateSmoothMoves(motorRightInfo);
	updateSmoothMoves(motorLeftInfo);
}

// Drive robot sideways
void driveSide(int speed) {
	setMotorTargetSpeed(motorFrontInfo, -speed * sin(90));
	setMotorTargetSpeed(motorBackInfo, speed * sin(90));

	updateSmoothMoves(motorFrontInfo);
	updateSmoothMoves(motorBackInfo);
}

// Sets the heading of the robot
void setRobotHeading(int degrees) {
	heading = degrees;
}

void rotate(int speed) {

	if (abs(speed) < 20) {
		speed = 0;
	} else {
		addToTargetSpeed(motorFrontInfo, speed);
		addToTargetSpeed(motorRightInfo, speed);
		addToTargetSpeed(motorBackInfo, speed);
		addToTargetSpeed(motorLeftInfo, speed);
	}
}

void sendRobot(TJoystick &joystick) {
	sendRobot(directionFromJoystick(joystick), speedFromJoystick(joystick));
}

void updateHolonomicDrive() {
	updateSmoothMoves(motorRightInfo);
	updateSmoothMoves(motorLeftInfo);
	updateSmoothMoves(motorFrontInfo);
	updateSmoothMoves(motorBackInfo);

	if (hasGyro) {
		updateGyro(gyro);
	}
}
