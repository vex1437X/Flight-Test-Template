#include "main.h"
#include "FTT/drive/drive.hpp"

using namespace ftt;

void Drive::set_drive_pid(double p, double i, double d){
    driveP = p;
    driveI = i;
    driveD = d;
}

void Drive::set_turn_pid(double p, double i, double d){
    turnP = p;
    turnI = i;
    turnD = d;
}

void Drive::reset_PID_values(){
    driveP = 0;
    driveI = 0;
    driveD = 0;
    turnP = 0;
    turnI = 0;
    turnD = 0;
}

void Drive::drive_pid(double target, double percent_speed){ // TODO: add heading correction
    reset_drive_sensors();
    int dir = (target < 0) ? -1: 1; // driving direction (+1 for forwards; -1 for backwards)
    double volt = percent_speed*1.27; // percent to voltage

    // convert target inches into target encoder
    double TPI = get_tick_per_inch();
    double tar = target * TPI; 
    
    double error = 0;
    double prevError = 0;
    double P = 0;
    double i = 0;
    double I = 0;
    double d = 0;
    double D = 0;

    double lPower = 0;
    double rPower = 0;

    double tolError = (target/100) * TPI; // error tolerance

    while ( !(((right_encs() + left_encs()) / 2) > tar - tolError && ((right_encs() + left_encs()) / 2) < tar + tolError)){
        prevError = error;
        error = tar - ((right_encs() + left_encs()) / 2);
        P = error * driveP;
        i += error;
        I = i * driveI;
        d = error - prevError;
        D = d * driveD;

        lPower = volt + P + I + D;
        rPower = volt + P + I + D;
        
        set_tank(lPower, rPower);
    }
}

void Drive::turn_pid(double target, double percent_speed, int direction){
    reset_drive_sensors();
    int dir = direction; // easier to type
    double volt = percent_speed*1.27; // percent to voltage

    // while
    right_motors.front().get_position();

    set_tank(volt*dir, volt*-dir);
}