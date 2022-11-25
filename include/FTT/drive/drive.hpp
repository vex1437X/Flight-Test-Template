#pragma once

#include <functional>
#include <iostream>
#include <tuple>

// #include "FTT/PID.hpp"
#include "FTT/util.hpp"
#include "pros/motors.h"

using namespace ftt;

class Drive {
 public:
    /**
     * Joysticks will be set to 0 when they are within this number.  
     * Set with set_controller_threshold()
    */
    int cont_thresh;

    /**
     * Vector of motors for the left chassis.
     */
    std::vector<pros::Motor> left_motors;

    /**
     * Vector of motors for the right chassis.
     */
    std::vector<pros::Motor> right_motors;

    /**
     * Inertial sensor.
     */
    pros::Imu imu;

    /**
     * Calibrates imu
     */
    void initialise();

    // /**
    //  * Tasks for auto.
    //  */
    // pros::Task fttauto;

    /**
     * Chassis constructor
     */
    Drive(std::vector<int> left_motor_ports, std::vector<int> right_motor_ports, int imu_port, double wheel_diameter, double cartridge_rpm, double ratio);

    /**
     * Sets the chassis to controller joysticks using tank control.  Run is usercontrol.
     */
    void tank();

    /**
     * Sets the chassis to voltage
     *
     * \param left
     *        voltage for left side, -127 to 127
     * \param right
     *        voltage for right side, -127 to 127
     */
    void set_tank(int left, int right);

    /**
     * Sets a new threshold for the controller.
     */
    void set_controller_threshold(int threshold);

    /**
     * Changes the brake type of the drive motors
     *
     * \param brake_type
     *        the 'brake mode' of the motor e.g. 'pros::E_MOTOR_BRAKE_COAST' 'pros::E_MOTOR_BRAKE_BRAKE' 'pros::E_MOTOR_BRAKE_HOLD'
     */
    void set_drive_brake(pros::motor_brake_mode_e_t brake_type);

    /**
     * Global current brake mode.
     * Default mode is COAST
     */
    pros::motor_brake_mode_e_t CURRENT_BRAKE = pros::E_MOTOR_BRAKE_COAST;

    /**
     * Resets the current gyro value.  Defaults to 0.
     *
     * \param new_heading
     *        New heading value.
     */
    void reset_gyro(double new_heading = 0);

    /**
     * Calibrates the inertial sensor.
     */
    bool imu_calibrate();

    /**
     * Returns current tick_per_inch()
     */
    double get_tick_per_inch();

    /**
     * Tick per inch calculation.
     */
    double TICK_PER_REV;
    double TICK_PER_INCH;
    double CIRCUMFERENCE;

    double CARTRIDGE;
    double RATIO;
    double WHEEL_DIAMETER;
};