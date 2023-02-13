#pragma once

#include <functional>
#include <iostream>
#include <tuple>

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
     * Amount of Left/Right drive motors
     * 
     */
    int NUM_LEFT_MOTOR;
    int NUM_RIGHT_MOTOR;

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
     * Sets the chassis to controller joysticks using tank control. Run in usercontrol.
     */
    void tank();

    /**
     * Sets the chassis to controller joysticks using arcade control. Run in usercontrol.
     */
    void arcade();

    /**
     * Sets the chassis to voltage
     *
     * @param left
     *        voltage for left side, -127 to 127
     * @param right
     *        voltage for right side, -127 to 127
     */
    void set_tank(int left, int right);

    /**
     * Sets a new threshold for the controller.
     */
    void set_controller_threshold(int threshold);

    /**
     * Controller threshold
     */
    double THRESH;

    /**
     * Changes the brake type of the drive motors
     *
     * @param brake_type
     *        the 'brake mode' of the motor e.g. 'pros::E_MOTOR_BRAKE_COAST' 'pros::E_MOTOR_BRAKE_BRAKE' 'pros::E_MOTOR_BRAKE_HOLD'
     */
    void set_drive_brake(pros::motor_brake_mode_e_t brake_type);

    /**
     * Global current brake mode.
     * Default mode is COAST
     */
    pros::motor_brake_mode_e_t CURRENT_BRAKE = pros::E_MOTOR_BRAKE_COAST;

    /**
     * Resets & tares drive motors
     */
    void reset_drive_sensors();

    /**
     * Resets the current gyro value.  Sets to 0.
     */
    void reset_gyro();

    /**
     * Sets the heading value of the inertial sensor.
     * Useful for long auton runs where you 'reset' your bot along a wall
     * 
     * @param heading
     *      Heading of gyro in degrees
     */
    void set_heading(int heading); 

    /**
     * @brief Get the locked heading of the robot.
     * 
     * @return double [0, 359.999]
     */
    double get_locked_heading();

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

    /**
     * Returns the absolute average position of the left chassis motors
     */
    double left_encs();

    /**
     * Returns the absolute average position of the right chassis motors
     */
    double right_encs();

    /**
     * Returns the absolute average position of the chassis motors
     */
    double encs();

    /**
     * PID VALUES
     * 
     */
    double driveP;
    double driveI;
    double driveD;
    double driveH;
    double turnP;
    double turnI;
    double turnD;

    /**
     * Set PID values for driving
     * forwards and backwards
     * 
     * @param p 
     * Proportional value
     * @param i 
     * Integral value
     * @param d 
     * Derivative value
     * @param heading
     * Heading multiplier (1.5-3x recommended)
     */
    void set_drive_pid(double p, double i, double d, double heading);

    /**
     * Set PID values for turning
     * left and right
     * 
     * @param p 
     * Proportional value
     * @param i 
     * Integral value
     * @param d 
     * Derivative value
     */
    void set_turn_pid(double p, double i, double d);

    /**
     * @brief 
     * Reset PID values for turning and driving.
     * 
     */
    void reset_PID_values();

    /**
     * Task for the PID loops being ran in Autonomous. Includes drive_pid and turn_pid
     */
    void auton_pid_task();

    /**
     * @brief 
     * Drive for target amount
     * @param target 
     * Target drive (in inches; -∞ to +∞)
     * @param percent_speed 
     * Driving speed (in percent; 0 to 100)
     */
    void drive_pid(double target, double percent_speed);

    /**
     * @brief 
     * Turns TO target (NOT FOR target degrees).
     * @param target 
     * Target turn (in degrees; 0 to 359.999)
     * @param percent_speed 
     * Turning speed (in percent; 0 to 100)
     */
    void turn_pid(double target, double percent_speed);

    /**
     * @brief
     * Stalls auton code until the chassis has stopped
     */
    void settle_drive();

    /**
     * @brief 
     * Sets all chassis motors to 0
     */
    void off();
};