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

void setSpeed(double leftSpeed, double rightSpeed) {
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
void setElevSpeed(double speed) {
	elev.setSpeed(speed);
}
void intake(double speed) {
	intakeMotor.setSpeed(speed);
}
};

class Shooter {
Motor shooter{15};
Motor ballPath{14};
Motor intakeMotor{13};
void setShooterSpeed(double speed) {
	shooter.setSpeed(speed);
}
void setBallPathSpeed(double speed) {
	shooter.setSpeed(speed);
}
void intake(double speed) {
	intakeMotor.setSpeed(speed);
}
};

int main() {
	serialib arduino;
	char errorOpening = arduino.openDevice("/dev/ttyACM0", 9600);
	if(errorOpening!=1) return errorOpening;
	Drive drive;
	char fromSerial[15] = "stuff\n";
	while(true) {
		arduino.readString(fromSerial, '\n', 14, 2000);
		printf("String read: %s\n", fromSerial);
		ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100);
	}
	arduino.closeDevice();
}
