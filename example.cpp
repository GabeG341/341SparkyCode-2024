#define Phoenix_No_WPI // remove WPI dependencies
#include "ctre/Phoenix.h"
#include "ctre/phoenix/platform/Platform.hpp"
#include "ctre/phoenix/unmanaged/Unmanaged.h"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include "serialib/lib/serialib.cpp"

using namespace ctre::phoenix;
using namespace ctre::phoenix::platform;
using namespace ctre::phoenix::motorcontrol;
using namespace ctre::phoenix::motorcontrol::can;


/* make some talon  ys for drive train */
std::string interface = "can0";

void sleepApp(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

struct serialInfo {
	double left;
	double right;
	double val1;
	double val2;
	double val3;
};

class Motor {
TalonSRX* motor;

public: Motor(int id) {
	motor = new TalonSRX(id, interface);
	double p = 1;
	double i = 0;
	double d = 0.1;
	motor->Config_kP(0, 1);
	motor->Config_kI(0, 0);
	motor->Config_kD(0, 0.1);
}
void setSpeed(double speed) {
	motor->Set(ControlMode::PercentOutput, speed);
}
void setVelocity(double speed) {
	motor->Set(ControlMode::Velocity, speed);
}
void setPosition(double pos) {
	motor->Set(ControlMode::Position, pos);
}
void enslave(int id) {
	motor->Set(ControlMode::Follower, id);
}
};

class Drive {
Motor motorLeft{20};
Motor motorRight{19};

public: void setSpeed(double leftSpeed, double rightSpeed) {
	motorLeft.setSpeed(leftSpeed);
	motorRight.setSpeed(rightSpeed);
}
};

class Elevator {
Motor elev{18};
Motor slaveElev{17};
Motor intakeMotor{16};

public: Elevator() {
	slaveElev.enslave(18);
}
void update(double val1, double val2, double val3) {
	if(val1 == 1) {
		setElevSpeed(val2);
	}
	else {
		setElevPosition(val2);
	}
	intake(val3);
}
private: void setElevSpeed(double speed) {
	elev.setSpeed(speed);
}
void setElevPosition(double pos) {
	elev.setPosition(pos);
}
void intake(double speed) {
	intakeMotor.setSpeed(speed);
}
};

class Shooter {
Motor shooter{15};
Motor ballPath{14};
Motor intakeMotor{13};

public: void update(double val1, double val2, double val3) {
	setShooterVelocity(val1);
	setBallPathSpeed(val2);
	intake(val3);
}
private: void setShooterSpeed(double speed) {
	shooter.setSpeed(speed);
}
void setShooterVelocity(double speed) {
	shooter.setVelocity(speed);
}
void setBallPathSpeed(double speed) {
	shooter.setSpeed(speed);
}
void intake(double speed) {
	intakeMotor.setSpeed(speed);
}
};

class Robot {
Drive drive;
Elevator elevator;
Shooter shooter;
int t;

public: Robot(int type) {
	t = type;
}
void update(serialInfo values) {
	drive.setSpeed(values.left, values.right);
	if(t == 1) {
		elevator.update(values.val1, values.val2, values.val3);
	}
	else {
		shooter.update(values.val1, values.val2, values.val3);
	}
}
};

int main() {
	serialib arduino;
	char errorOpening = arduino.openDevice("/dev/ttyACM0", 9600);
	if(errorOpening!=1) return errorOpening;
	Robot robot(1);
	char fromSerial[101] = "stuff\n";
	while(true) {
		serialInfo values;
		std::string cereal = "";
		int t = 0;
		arduino.readString(fromSerial, '\n', 100, 2000);
		for(int i = 0; i < 101; i++) {
			if(fromSerial[i] == '\0') {
				switch(t) {
					case 0:
						values.left = std::stod(cereal);
						break;
					case 1:
						values.right = std::stod(cereal);
						break;
					case 2:
						values.val1 = std::stod(cereal);
						break;
					case 3:
						values.val2 = std::stod(cereal);
						break;
					case 4:
						values.val3 = std::stod(cereal);
						break;
				}
				break;
			}
			if(fromSerial[i] != ',') {
				cereal += fromSerial[i];
			}
			else {
				switch(t) {
					case 0:
						values.left = std::stod(cereal);
						break;
					case 1:
						values.right = std::stod(cereal);
						break;
					case 2:
						values.val1 = std::stod(cereal);
						break;
					case 3:
						values.val2 = std::stod(cereal);
						break;
					case 4:
						values.val3 = std::stod(cereal);
						break;
				}
				t++;
				cereal.clear();
			}
		}
		printf("Left motor: %f\n", values.left);
		printf("Right motor: %f\n", values.right);
		printf("Motor value 1: %f\n", values.val1);
		printf("Motor value 2: %f\n", values.val2);
		printf("Motor value 3: %f\n", values.val3);
		//printf("Everything: %s\n", fromSerial);
		ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100);
		robot.update(values);
	}
	arduino.closeDevice();
}
