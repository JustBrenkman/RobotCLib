// This is a smooth motor controller for motors
// It also includes use of PID
// And the encoders of the motors

typedef struct MotorInfo;
typedef struct ServoInfo;
void setMotorTargetSpeed(MotorInfo motorInfo, int speed);

typedef enum {
	MOTOR = 1,
	SERVO = 2
} DRIVETYPE;

typedef struct {
	float targetSpeed;
	// Tells the speed controller how fast to accelerate
	float speedIncrement;
	int motorPort;
	DRIVETYPE driveType;
} MotorInfo;

typedef struct {
	// Speed at which the servo is moving
	float  speed;
	// rate of change of the speed of the servo
	float speedIncrement;

	//Read only value
	float servoTargetPosition;
	// Old Servo Position
	float servoPreTargetPosition;
	// Servo Position -Deprecated
	float servoPosition;

	// Servo Port indentifier
	TServoIndex servoPort;
} ServoInfo;

// Motor API
void setMotorTargetSpeed(MotorInfo motorInfo, int speed) {
	motorInfo.targetSpeed = speed;
}

void addToTargetSpeed(MotorInfo motorInfo, int add) {
	motorInfo.targetSpeed += add;
}

void multiplyMotorTargetSpeed(MotorInfo motorInfo, int multiplyer) {
	motorInfo.targetSpeed *= multiplyer;
}

void setMotorSpeedIncrement(MotorInfo motorInfo, float increment) {
	motorInfo.speedIncrement = increment;
}

//Servo API

void setServoPosition(ServoInfo servoInfo, int servoPosition) {
	servoInfo.servoTargetPosition = servoPosition;
}

void addToServoPosition(ServoInfo servoInfo, int add) {
	servoInfo.servoTargetPosition += add;
}

void setServoSpeedIncrement(ServoInfo servoInfo, int increment) {
	servoInfo.speedIncrement = increment;
}


// Update API's

void updateSmoothMoves(MotorInfo infoMotor) {
	if (motor[infoMotor.motorPort] < infoMotor.targetSpeed) {
		motor[infoMotor.motorPort] += infoMotor.speedIncrement;
	} else if (motor[infoMotor.motorPort] > infoMotor.targetSpeed) {
		motor[infoMotor.motorPort] -= infoMotor.speedIncrement;
	}
}

// Servo update API - updates the servo speed for smothness (hence SmoothMoves) and the position of the servo
void updateSmoothMoves(ServoInfo &servoInfo) {
	// Update the position of the servo
}
