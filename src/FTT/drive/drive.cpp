#include "main.h"
#include "FTT/drive/drive.hpp"

using namespace ftt;

Drive::Drive(std::vector<int> left_motor_ports, std::vector<int> right_motor_ports, 
            int imu_port, double wheel_diameter, double cartridge_rpm, double ratio): 
            imu(imu_port) /* initialise inertial */ {
    NUM_LEFT_MOTOR = 0;
    NUM_RIGHT_MOTOR = 0;

    // Set ports to a global vector
    // Creates motors
    // Initialises how many motors there are on each side of the chassis
    for (int i  = 0; i < left_motor_ports.size(); i++) {
        NUM_LEFT_MOTOR++;
        pros::Motor place(i, is_reversed(i));
        left_motors.push_back(place);
    }
    for (int i  = 0; i < right_motor_ports.size(); i++) {
        NUM_RIGHT_MOTOR++;
        pros::Motor place(i, is_reversed(i));
        right_motors.push_back(place);
    }

    

    // Set constants for tick_per_inch calculation
    WHEEL_DIAMETER = wheel_diameter;
    RATIO = ratio;
    CARTRIDGE = cartridge_rpm;
    TICK_PER_INCH = get_tick_per_inch();
}

double Drive::get_tick_per_inch() {
    CIRCUMFERENCE = WHEEL_DIAMETER * M_PI;

    TICK_PER_REV = (50.0 * (3600.0 / CARTRIDGE)) * RATIO;

    TICK_PER_INCH = (TICK_PER_REV / CIRCUMFERENCE);
    return TICK_PER_INCH;
}

void Drive::set_tank(int left, int right) {
    for (pros::Motor i : left_motors) {
        i.move(left);
    }
    for (pros::Motor i : right_motors) {
        i.move(right);
    }
}

bool Drive::imu_calibrate() {
    imu.reset();
    int i;
    for (i = 0; imu.is_calibrating(); i+=10){
        if (i >= 3000){ // imu needs about 2000ms to calibrate; so if it takes longer; no imu
            printf("No IMU plugged in, (took %d ms to realize that)\n", i);
            return false;
        }
        pros::delay(10);
    }

    printf("IMU is done calibrating (took %d ms)\n", i);
    return true;
}

void Drive::reset_gyro(){ 
    imu.set_heading(0);
}

void Drive::set_heading(int heading){ 
    imu.set_heading(heading);
}

void Drive::set_drive_brake(pros::motor_brake_mode_e_t brake_type) {
    CURRENT_BRAKE = brake_type;
    for (pros::Motor i : left_motors) {
        i.set_brake_mode(brake_type);
    }
    for (pros::Motor i : right_motors) {
        i.set_brake_mode(brake_type);
    }
}

void Drive::initialise() {
    imu_calibrate();
    reset_drive_sensors();
}

void Drive::set_controller_threshold(int threshold){
    THRESH = abs(threshold);
}

void Drive::tank() {
    reset_drive_sensors(); // not really necessary

    int l_stick = (abs(master.get_analog(ANALOG_LEFT_Y)) < THRESH) ? 0 : master.get_analog(ANALOG_LEFT_Y);
    int r_stick = (abs(master.get_analog(ANALOG_RIGHT_Y)) < THRESH) ? 0 : master.get_analog(ANALOG_RIGHT_Y);

    set_tank(l_stick, r_stick);
}

void Drive::reset_drive_sensors() {
    for (pros::Motor i : left_motors) {
        i.tare_position();
    }
    for (pros::Motor i : right_motors) {
        i.tare_position();
    }
}

double Drive::left_encs() {
    double left = 0.0;
    for (pros::Motor i : left_motors) {
        left += i.get_position();
    }
    return left/NUM_LEFT_MOTOR;
}

double Drive::right_encs() {
    double right = 0.0;
    for (pros::Motor i : right_motors) {
        right += i.get_position();
    }
    return right/NUM_RIGHT_MOTOR;
}

double Drive::encs() {
    return (right_encs() + left_encs()) / 2;
}

double Drive::off() {
    set_tank(0, 0);
}