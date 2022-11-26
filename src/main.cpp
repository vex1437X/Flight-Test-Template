#include "main.h"
#include "pros/misc.h"

using namespace pros;
using namespace ftt;

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
	delay(500);

	as::auton_selector.add_autons({
		Auton("Auton1", auton1),
		Auton("Auton2", auton2),
		Auton("Auton3", auton3)
	});

	// initialise chassis and auton selector
	chassis.initialise();
	chassis.reset_PID_values();
	as::initialise();
}


void disabled() {}

void competition_initialize() {}

void autonomous() {
	chassis.reset_gyro(); // Reset gyro position to 0
	chassis.reset_drive_sensors(); // Reset drive sensors to 0
	chassis.set_drive_brake(MOTOR_BRAKE_HOLD); // Set motors to hold.  This helps autonomous consistency.

	as::auton_selector.call_selected_auton(); // Calls selected auton from autonomous selector.
}

void opcontrol() {
	chassis.set_drive_brake(MOTOR_BRAKE_BRAKE);
	while (true) {
		chassis.tank(); // Tank control

		delay(10);
	}
}
