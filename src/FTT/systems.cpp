#include "main.h"
#include "FTT/systems.hpp"
#include "pros/motors.h"

using namespace ftt;

Systems::Systems(std::vector<int> intake_motor_ports, std::vector<int> colour_motor_ports, 
        std::vector<int> catapult_motor_ports, std::vector<int> flywheel_motor_ports, int flywheel_CARTRIDGE, 
        int catapult_limit_switch_port, int plate_optical_port, int colourwheel_optical_port, std::vector<int> expansion_ports){
    
    // Set ports to global motor vectors
    for (int i : intake_motor_ports) {
        pros::Motor place(abs(i), is_reversed(i));
        intake_motors.push_back(place);
    }
    for (int i : colour_motor_ports) {
        pros::Motor place(abs(i), is_reversed(i));
        colour_motors.push_back(place);
    }
    for (int i : catapult_motor_ports) {
        pros::Motor place(abs(i), is_reversed(i));
        catapult_motors.push_back(place);
    }
    for (int i : flywheel_motor_ports) {
        NUM_FLYWHEEL++;
        pros::Motor place(abs(i), is_reversed(i));
        flywheel_motors.push_back(place);
    }
    for (int i : expansion_ports) {
        pros::ADIDigitalOut place(abs(i));
        expansion_pneumatics.push_back(place);
    }
    // Set limit switch port
    pros::ADIDigitalIn place(catapult_limit_switch_port);
    cataLimit.push_back(place);

    // Set optical ports

    pros::Optical s1(plate_optical_port);
    plateColour.push_back(s1);

    pros::Optical s2(colourwheel_optical_port);
    colourW.push_back(s2);

    FLY_CART = flywheel_CARTRIDGE;
}

void Systems::set_intake(double percent) {
    for (pros::Motor i : intake_motors) {
        i.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
        i.move(127*(percent/100));
    }
}

void Systems::set_colour(double percent) {
    for (pros::Motor i : colour_motors) {
        i.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
        i.move(127*(percent/100));
    }
}

void Systems::set_cata(double percent) {
    for (pros::Motor i : catapult_motors) {
        i.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
        i.move(127*(percent/100));
    }
}

void Systems::set_cata_mode(pros::motor_brake_mode_e brake_mode) {
    for (pros::Motor i : catapult_motors) {
        i.set_brake_mode(brake_mode);
    }
}

void Systems::set_fly(double percent) {
    for (pros::Motor i : flywheel_motors) {
        i.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
        i.move(127*(percent/100));
    }
}

void Systems::set_expansion_pneumatics(bool value){
    for (pros::ADIDigitalOut i : expansion_pneumatics) {
        i.set_value(value);
    }
}

bool intaketoggleIN = false;
bool intaketoggleOUT = false;
bool flytoggle = false;
bool expand = false;

void Systems::Systems_control() {
    if (master.get_digital(INTAKE_IN)){
        if (intaketoggleIN == false){
            set_intake(abs(INTAKE_IN_SPEED));
            intaketoggleIN = true;
        } else if (intaketoggleIN == true){
            set_intake(0);
            intaketoggleIN = false;
        }
        pros::delay(350);
    }

    if (master.get_digital(INTAKE_OUT)){
        if (intaketoggleOUT == false){
            set_intake(abs(INTAKE_OUT_SPEED)*-1);
            intaketoggleOUT = true;
        } else if (intaketoggleOUT == true){
            set_intake(0);
            intaketoggleOUT = false;
        }
        pros::delay(350);
    }

    if (master.get_digital(FLYWHEEL_SPIN)){
        if (flytoggle == false){
            flytoggle = true;
            set_fly(abs(FLY_SPEED));
        } else if (flytoggle == true){
            flytoggle = false;
            set_fly(0);
        }
        pros::delay(350);
    }

    if (master.get_digital(EXPAND)){
        if (expand == false){
            set_expansion_pneumatics(true);
            expand = true;
        } else if (expand == true){
            set_expansion_pneumatics(false);
            expand = false;
        }
        pros::delay(350);
    }
}

double Systems::getActRPM(){
    double rpm = 0.0;
    for (pros::Motor i : flywheel_motors) {
        rpm += i.get_actual_velocity();
    }
    return rpm/NUM_FLYWHEEL;
}

// private
bool didReach = false;

int Systems::get_cataLimit_value(){
    for (pros::ADIDigitalIn i : cataLimit) {
        return i.get_value(); // should only run once
    }
    return -1;
}
void Systems::cataFor(double sec, int percent){
  set_cata(percent);
  pros::delay(sec*1000);
  set_cata(0);
}

void Systems::resetCata(){
    while(!didReach){
        set_cata_mode(MOTOR_BRAKE_COAST);
        set_cata(abs(100)*-1);
        if (get_cataLimit_value()){
            didReach = true;
            set_cata(0);
            pros::delay(10);
            set_cata_mode(MOTOR_BRAKE_BRAKE);
            pros::delay(1000);
            set_cata_mode(MOTOR_BRAKE_COAST);
            return;   
        }
        pros::delay(10);
    }
}

void Systems::fireCata(){
  if(didReach){
    set_cata_mode(MOTOR_BRAKE_COAST);
    set_intake(0);
    cataFor(0.2, -100);
  }
  didReach = false;
}

bool colourtoggle = false;
double redHue = 0;
double blueHue = 240;

int Systems::get_colourW_prox(){
    for (pros::Optical i : colourW) {
        return i.get_proximity();
    }
    return -1;
}


double Systems::get_colourW_hue(){
    for (pros::Optical i : colourW) {
        i.disable_gesture();
        i.set_led_pwm(100);
        return i.get_hue(); // should only run once
    }
    return -1.0;
}

double Systems::get_plate_hue(){
    for (pros::Optical i : plateColour) {
        i.disable_gesture();
        i.set_led_pwm(100);
        return i.get_hue(); // should only run once
    }
    return -1.0;
}

void Systems::spinRed(){ // spin until blue is bottom
    int exit = 0;
    while(((get_colourW_hue() > blueHue - 100 && get_colourW_hue() < blueHue + 80)) && exit < 1000){
        set_colour(abs(COLOUR_SPEED)*-1);
        exit++;
        pros::delay(10);
    }
    set_colour(0);
}

void Systems::spinBlue(){ // spin until red is bottom
    int exit = 0;
    while(((get_colourW_hue() > 360 - 40 || get_colourW_hue() < redHue + 100)) && exit < 1000){
        set_colour(abs(COLOUR_SPEED)*-1);
        exit++;
        pros::delay(10);
    }
    set_colour(0);
}

bool Systems::isRed(){
    if (get_plate_hue() > 360 - 40 || get_plate_hue() < redHue + 100){
        return true;
    } else {
        return false;
    }
}

void Systems::spinColour(){
    if (isRed()){
        spinRed();
    } else {
        spinBlue();
    }
}

double targetRPM = 0;
double mV = 0;
double exMV = 0;
bool fst = true;

void Systems::Systems_task() { // BEING RAN IN MAIN
    if(fst){ pros::delay(500); fst = false;} // allow initialise time before activating anything

    // Simple P loop for flywheel INTERNAL RPM
    if (flytoggle){
        // * voltage/rpm correction *
        // percent to miliVolts
        FLY_SPEED = abs(FLY_SPEED);
        targetRPM = FLY_CART*(FLY_SPEED/100); // FLY_CART is max internal rpm :: total rpm of system is 600*5/1 = 3000 rpm
        mV = 12000*(FLY_SPEED/100);       // 12000 is max mV          20 mV : 1 rpm
        exMV = 0;                 // extra mV needed to be added to flywheel
        if (getActRPM() < targetRPM-5 || getActRPM() > targetRPM+5) {exMV = 10*(targetRPM - getActRPM());}
        for (pros::Motor i : flywheel_motors) {
            i.move_voltage(mV + exMV);
        }
    }

    // Catapult control

    if (master.get_digital(CATA_SHOT)){
        fireCata();
        pros::delay(600);
    }
    resetCata();

    // Colour wheel control

    if (master.get_digital(COLOUR_SPIN)){
        // if (get_colourW_prox() < 15){
            spinColour();
        // }
    }

    pros::delay(10);
}

void Systems::set_flywheel_speed_percent(double percent) {
    FLY_SPEED = percent;
}

void Systems::set_intake_IN_speed(double percent) {
    INTAKE_IN_SPEED = percent;
}

void Systems::set_intake_OUT_speed(double percent) {
    INTAKE_OUT_SPEED = percent;
}

void Systems::set_colourwheel_speed(double percent) {
    COLOUR_SPEED = percent;
}

void Systems::set_intake_buttons(pros::controller_digital_e_t in, pros::controller_digital_e_t out) {
    INTAKE_IN = in;
    INTAKE_OUT = out;
}

void Systems::set_colourwheel_button(pros::controller_digital_e_t button) {
    COLOUR_SPIN = button;
}

void Systems::set_cata_button(pros::controller_digital_e_t button) {
    CATA_SHOT = button;
}

void Systems::set_flywheel_button(pros::controller_digital_e_t button) {
    FLYWHEEL_SPIN = button;
}

void Systems::set_expansion_button(pros::controller_digital_e_t button) {
    EXPAND = button;
}