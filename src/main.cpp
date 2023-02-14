#include "main.h"
#include "pros/misc.h"

using namespace pros;
using namespace ftt;

Drive chassis(
	// Left Chassis Ports (negative port will reverse)
	// Use {0, 0, 0}, {0, 0, 0, 0, ...} to support a 4, 6, 8, ... motor drives
	{0, 0}, // {0, 0, 0}

	// Right Chassis Ports (negative port will reverse)
	// Use {0, 0, 0}, {0, 0, 0, 0, ...} to support a 4, 6, 8, ... motor drives
	{0, 0}, // {0, 0, 0}

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

Systems sys( // Leave port at 0 if that specific subsystem is not present
	// Intake Ports (negative port will reverse)
	{0, 0},

	// Colourwheel Ports (negative port will reverse)
	{0, 0},

	// Catapult Ports (negative port will reverse)
	{0},

	// Flywheel Ports (negative port will reverse)
	{0, 0},

	// Flywheel CARTRIDGE (100 (red), 200 (green), 600 (blue))
	0,

	// Catapult limitswitch port
	// A = 1; B = 2; C = 3; etc...
	0,

	// License plate optical sensor port
	0,

	// Colour wheel optical sensor port
	// Place so colour wheel optical sensor is parallel with the inside of the colour wheel
	0,

	// Distance sensor port
	// Place to be parallel to the wall you will line up with when using the colour wheel
	0,

	// Expansion eneumatics ports
	// A = 1; B = 2; C = 3; etc...
	{0}
);

Skills skl(
	// GPS Sensor Port
	0
);

void Selector_task(void*){ // just a feeder function // KEEP THIS OTHERWISE AUTON SELECTOR WONT WORK
	while (true){
		as::auton_selector.run();
	}
}

void Auton_task(void*){ // just a feeder function // KEEP THIS OTHERWISE PID WONT WORK DURING AUTON
	while (true){
		chassis.auton_pid_task();
	}
}
void Systems_task(void*){ // just a feeder function // KEEP THIS OTHERWISE SYSTEMS WONT WORK DURING TELEOP
	while (true){
		sys.Systems_task();
	}
}

Task AutonsPID(Auton_task, nullptr); // always make sure this is before your auton is being called
Task SystemsCalc(Systems_task, nullptr); // used for flywheel adjustments // used for catapult resets // used for colour wheel
Task Selector(Selector_task, nullptr);

void initialize() {
	Selector.suspend();
	AutonsPID.suspend();
	SystemsCalc.suspend();
	delay(500);


	as::auton_selector.add_autons({
		Auton("Auton1", auton1),
		Auton("Auton2", auton2),
		Auton("Auton3", auton3)
	});

	// initialise chassis and auton selector
	chassis.initialise();
	chassis.reset_PID_values();
	skl.initialise();
	as::auton_selector.initialise();
	Selector.resume();
}


void disabled() {}

void competition_initialize() {}

void autonomous() {
	Selector.suspend();
	chassis.reset_gyro(); // Reset gyro position to 0
	chassis.reset_drive_sensors(); // Reset drive sensors to 0
	chassis.set_drive_brake(MOTOR_BRAKE_HOLD); // Set motors to hold.  This helps autonomous consistency.
	AutonsPID.resume();
	SystemsCalc.resume();
	as::auton_selector.call_selected_auton(); // Calls selected auton from autonomous selector.
}



void opcontrol() {
	Selector.suspend();
	Selector.remove();
	SystemsCalc.resume();
	AutonsPID.suspend(); // stop the auton PID
	AutonsPID.remove();

	chassis.set_drive_brake(MOTOR_BRAKE_HOLD); // change to whatever brake type (HOLD, COAST)
	chassis.set_controller_threshold(0);	   // controller threshold -- limits controller drift

	// Set system controls
	sys.set_intake_buttons(DIGITAL_A, DIGITAL_A);
	sys.set_colourwheel_button(DIGITAL_A);
	sys.set_cata_button(DIGITAL_A);
	sys.set_flywheel_button(DIGITAL_A);

	// Set system speeds
	// ALL IN PERCENT (0 to 100)
	sys.set_intake_IN_speed(0);
	sys.set_intake_OUT_speed(0);
	sys.set_colourwheel_speed(0);
	sys.set_flywheel_speed_percent(0);
	
	while (true) {
		chassis.tank(); // tank control
		// chassis.arcade(); // arcade control
		sys.Systems_control(); // Systems control

		delay(10);
	}
}
