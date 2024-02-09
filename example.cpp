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
//#include "serial/serial.sh"

using namespace ctre::phoenix;
using namespace ctre::phoenix::platform;
using namespace ctre::phoenix::motorcontrol;
using namespace ctre::phoenix::motorcontrol::can;

/* make some talon  ys for drive train */
std::string interface = "can0";

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
};

class Drive {
Motor motorLeft{20};
Motor motorRight{19};

void drive(double leftSpeed, double rightSpeed) {
	motorLeft.setSpeed(leftSpeed);
	motorRight.setSpeed(rightSpeed);
}
};

void sleepApp(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main() {
	Drive drive;
	while (true) {
		ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100);
	}
}
