#pragma once

#include <functional>
#include <iostream>
#include <tuple>

// #include "FTT/PID.hpp"
#include "FTT/util.hpp"
#include "pros/adi.hpp"
#include "pros/motors.h"

using namespace ftt;

class Systems {
 public:
    /**
     * Vector of motors for the intake motors.
     */
    std::vector<pros::Motor> intake_motors;

    /**
     * Vector of motors for the colourwheel motors.
     */
    std::vector<pros::Motor> colour_motors;

    /**
     * Vector of motors for the catapult motors.
     */
    std::vector<pros::Motor> catapult_motors;

    /**
     * Vector of motors for the flywheel motors.
     */
    std::vector<pros::Motor> flywheel_motors;
   
    /**
     * Vector of ports for SpinUp expansion
     */
    std::vector<pros::ADIDigitalOut> expansion_pneumatics;

    /**
     * Catapult limit switch
     */
    std::vector<pros::ADIDigitalIn> cataLimit;

    /**
     * Platenumber optical sensor
     */
    std::vector<pros::Optical> plateColour;

    /**
     * Colourwheel optical sensor
     */
    std::vector<pros::Optical> colourW;

   /**
    * Distance sensor for colour wheel
    */
    std::vector<pros::Distance> distanceColour;

    /**
     * Systems constructor
     * Set port # to '0' if that type of motor is not present
     */
    Systems(std::vector<int> intake_motor_ports, std::vector<int> colour_motor_ports, 
            std::vector<int> catapult_motor_ports, std::vector<int> flywheel_motor_ports, 
            int flywheel_CARTRIDGE, int catapult_limit_switch_port, int plate_optical_port, 
            int colourwheel_optical_port, int distance_port, std::vector<int> expansion_ports);

     /**
     * Sets the chassis to voltage
     *
     * @param left
     *        voltage for left side, -127 to 127
     * @param right
     *        voltage for right side, -127 to 127
     */
    void set_tank(int left, int right);

    /***
     * Sets the intake percent
     * 
     * @param percent
     * percent speed for intake, -100 to 100
    */
    void set_intake(double percent);

    /***
     * Sets the colourwheel percent
     * 
     * @param percent
     * percent speed for colourwheel, -100 to 100
    */
    void set_colour(double percent);
    
    /***
     * Sets the catapult percent
     * 
     * @param percent
     * percent speed for catapult, -100 to 100
    */
    void set_cata(double percent);

    /***
     * Sets the flywheel percent
     * 
     * @param percent
     * percent speed for flywheel, -100 to 100
    */
    void set_fly(double percent);

    /**
    * @brief Sets the mode for the expansion pneumatics
    * 
    * @param value 
    * true/false ----
    * true - on || false - off
    */
    void set_expansion_pneumatics(bool value);


    /***
     * Control systems. Use in driver control.
    */
    void Systems_control();

    /**
     * Global current toggle intake IN button.
     */
    pros::controller_digital_e_t INTAKE_IN;

    /**
     * Global current toggle intake OUT button.
     */
    pros::controller_digital_e_t INTAKE_OUT;

    /**
     * Global current toggle colourwheel SPIN button.
     */
    pros::controller_digital_e_t COLOUR_SPIN;

    /**
     * Global current catapult SHOOT button.
     */
    pros::controller_digital_e_t CATA_SHOT;

    /**
     * Global current toggle flywheel button.
     */
    pros::controller_digital_e_t FLYWHEEL_SPIN;

    /**
     * Global current expansion button
     */
    pros::controller_digital_e_t EXPAND;

    

    /***
     * Sets the buttons to toggle the intake motors
     * @param in
     * Intake button
     * @param out
     * Outtake button
     * 
    */
    void set_intake_buttons(pros::controller_digital_e_t in, pros::controller_digital_e_t out);

    /***
     * Sets the button to spin the colourwheel motors
    */
    void set_colourwheel_button(pros::controller_digital_e_t button);

    /***
     * Sets the button to shoot the catapult
    */
    void set_cata_button(pros::controller_digital_e_t button);

    /***
     * Sets the button to spin the flywheel motors
    */
    void set_flywheel_button(pros::controller_digital_e_t button);

    /***
     * Sets the button to activate the expansion pneumatics
    */
    void set_expansion_button(pros::controller_digital_e_t button);

    /***
     * Systems task. Used for calculations and adjustments for flywheel & catapult. Use in driver control.
    */
    void Systems_task();

    /***
     * Driver control flywheel speed
    */
    double FLY_SPEED = 0;

    /***
     * Driver control intake IN speed
    */
    double INTAKE_IN_SPEED = 0;

    /***
     * Driver control intake OUT speed
    */
    double INTAKE_OUT_SPEED = 0;

    /***
     * Driver control colourwheel spin speed
    */
    double COLOUR_SPEED = 0;

    /***
     * Set driver control flywheel speed
     * @param percent
     *  in percent
    */
    void set_flywheel_speed_percent(double percent);

    /***
     * Set driver control intake IN speed
     * @param percent
     *  in percent
    */
    void set_intake_IN_speed(double percent);

    /***
     * Set driver control intake OUT speed
     * @param percent
     *  in percent
    */
    void set_intake_OUT_speed(double percent);

    /***
     * Set driver control colourwheel speed
     * @param percent
     *  in percent
    */
    void set_colourwheel_speed(double percent);

   /**
    * @brief Set the brake type on the catapult motors
    * 
    * @param brake_mode 
    */
    void set_cata_mode(pros::motor_brake_mode_e brake_mode);

   int NUM_FLYWHEEL = 0;
   int NUM_CATAPULT = 0;
   int NUM_INTAKE = 0;
   int NUM_COLOURWHEEL = 0;

   bool contains_flywheel();
   bool contains_catapult();
   bool contains_intake();
   bool contains_colourwheel();

 private:
    double getActRPM();

    int FLY_CART = 0;

    int get_cataLimit_value();
    void cataFor(double sec, int percent);
    void resetCata();
    void fireCata();

    int get_colourW_prox();
    double get_colourW_hue();
    double get_plate_hue();
    void spinRed();
    void spinBlue();
    double get_distance();
    bool isRed();
    void spinColour();
};