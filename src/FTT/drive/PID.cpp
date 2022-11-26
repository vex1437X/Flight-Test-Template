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

// TODO: Make drive and turn PID into tasks
void Drive::drive_pid(double target, double percent_speed){
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

    double headingInit = imu.get_heading();
    double headingError = 0;
    int H = 0;



    while ( !(((right_encs() + left_encs()) / 2) > tar - tolError && ((right_encs() + left_encs()) / 2) < tar + tolError)){
        prevError = error;
        error = tar - ((right_encs() + left_encs()) / 2);
        headingError = headingInit - imu.get_heading(); // + --> right more power | - --> left more power
        if (headingError < -180){headingError += 360;}
        else if(headingError > 180){headingError -= 360;}

        i += error;
        d = error - prevError;

        P = error * driveP;
        I = i * driveI;
        D = d * driveD;
        H = headingError*2.5;

        dir = (error < 0) ? -1 : 1;
        lPower = dir*(volt + P + I + D - H);
        rPower = dir*(volt + P + I + D + H);
        
        set_tank(lPower, rPower);
    }

    // Overshoot correction
    while ((((right_encs() + left_encs()) / 2) > tar + tolError)){
        prevError = error;
        error = tar - ((right_encs() + left_encs()) / 2);

        i += error;
        d = error - prevError;

        P = error * driveP;
        I = i * driveI;
        D = d * driveD;

        dir = (error < 0) ? -1 : 1; // should be -1
        lPower = dir*(volt + P + I + D);
        rPower = dir*(volt + P + I + D);
        
        set_tank(lPower, rPower);
    }
    // Undershoot correction
     while ((((right_encs() + left_encs()) / 2) < tar - tolError)){
        prevError = error;
        error = tar - ((right_encs() + left_encs()) / 2);

        i += error;
        d = error - prevError;

        P = error * driveP;
        I = i * driveI;
        D = d * driveD;

        dir = (error < 0) ? -1 : 1; // should be 1
        lPower = dir*(volt + P + I + D);
        rPower = dir*(volt + P + I + D);
        
        set_tank(lPower, rPower);
    }
}

void Drive::turn_pid(double target, double percent_speed, int direction){
    reset_drive_sensors();
    double volt = percent_speed*1.27; // percent to voltage

    double error = 0;
    double prevError = 0;
    double P = 0;
    double i = 0;
    double I = 0;
    double d = 0;
    double D = 0;

    double lPower = 0;
    double rPower = 0;

    double tolError = 0.05; // error tolerance

    while (!(imu.get_heading() > target - tolError && imu.get_heading() < target + tolError)){
        prevError = error;
        error = target - imu.get_heading();

        i += error;
        d = error - prevError;

        P = error * turnP;
        I = i * turnI;
        D = d * turnD;

        lPower = (volt + P + I + D)*direction;
        rPower = (volt + P + I + D)*-direction;
        
        set_tank(lPower, rPower);
    }
    // Overshoot correction
    while (imu.get_heading() > target + tolError){
        prevError = error;
        error = target - imu.get_heading();

        i += error;
        d = error - prevError;

        P = error * turnP;
        I = i * turnI;
        D = d * turnD;

        lPower = (volt + P + I + D)*-direction;
        rPower = (volt + P + I + D)*direction;
        
        set_tank(lPower, rPower);
    }

    // Undershoot correcton
    while (imu.get_heading() < target - tolError){
        prevError = error;
        error = target - imu.get_heading();

        i += error;
        d = error - prevError;

        P = error * turnP;
        I = i * turnI;
        D = d * turnD;

        lPower = (volt + P + I + D)*direction;
        rPower = (volt + P + I + D)*-direction;
        
        set_tank(lPower, rPower);
    }
}

void Drive::settle_drive(){
    reset_drive_sensors();
    double prev = -10;
    double curr = (right_encs() + left_encs()) / 2;
    while(prev != curr){
        prev = curr;
        curr = (right_encs() + left_encs()) / 2;
        pros::delay(20);
    }
}