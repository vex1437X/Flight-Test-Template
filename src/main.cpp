#include "main.h"
#include "pros/misc.h"

using namespace pros;

Drive chassis(
	// Left Chassis Ports (negative port will reverse it!)
	{0, 0},

	// Right Chassis Ports (negative port will reverse)
	{0, 0},

	// IMU Port
	0,

	// Drive Wheel Diameter
	0,

	// Cartridge RPM
	// 100 (red) , 200 (green) , 600 (blue)
	0,

	// External Gear Ratio (decimal ratio)
	// if your drive is 12:36 where the 36t gear is powered, your ratio would be **0.6**.
	0.0
);

void initialize() {
	lcd::initialize();
	chassis.initialise();
}


void disabled() {}

void competition_initialize() {}

void autonomous() {

}

void opcontrol() {

	while (true) {
		chassis.tank(); // Tank control

		delay(10);
	}
}
