#include "main.h"
#include "FTT/drive/drive.hpp"

using namespace ftt;

Drive::Drive(std::vector<int> left_motor_ports, std::vector<int> right_motor_ports, 
            int imu_port, double wheel_diameter, double cartridge_rpm, double ratio): 
            imu(imu_port) /* initialise inertial */ {

    // Set ports to a global vector
    // Creates motors
    for (int i  = 0; i < left_motor_ports.size(); i++) {
        pros::Motor place(i, is_reversed(i));
        left_motors.push_back(place);
    }
    for (int i  = 0; i < right_motor_ports.size(); i++) {
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
        i.move_voltage(left * (12000.0 / 127.0));
    }
    for (pros::Motor i : right_motors) {
        i.move_voltage(right * (12000.0 / 127.0));
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
}