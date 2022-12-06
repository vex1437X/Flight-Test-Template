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

void Drive::drive_pid(double target, double percent_speed){
    reset_drive_sensors();
    int dir = (target < 0) ? -1: 1; // driving direction (+1 for forwards; -1 for backwards)
    double volt = abs(percent_speed*1.27)*dir; // percent to voltage

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

    double O = 0;
    double Ol = 0;
    double Or = 0;

    double lPower = 0;
    double rPower = 0;

    double tolError = (target/100) * TPI; // error tolerance

    double headingInit = imu.get_heading();
    double headingError = 0;
    int H = 0;



    while ( !(encs() > tar - tolError && encs() < tar + tolError)){
        prevError = error;
        error = tar - encs();
        headingError = headingInit - imu.get_heading(); // + --> right more power | - --> left more power
        if (headingError < -180){headingError += 360;}
        else if(headingError > 180){headingError -= 360;}

        i += error;
        d = error - prevError;

        P = error * driveP;
        I = i * driveI;
        D = d * driveD;
        H = headingError;

        printf("P: %f \n", P);
        printf("I: %f\n", I);
        printf("D: %f\n", D);

        // lPower = volt + P + I + D - H;
        // rPower = volt + P + I + D + H;

        // limit max speed
        Ol = (abs(P + I + D - H) > abs(volt)) ? abs(volt)*dir : abs(P + I + D - H)*dir;
        Or = (abs(P + I + D + H) > abs(volt)) ? abs(volt)*dir : abs(P + I + D + H)*dir;
        printf("O1: %f\n", O);

        lPower = Ol;
        rPower = Or;

        set_tank(lPower, rPower);
    }

    // Overshoot correction
    while ((encs() > tar + tolError)){
        prevError = error;
        error = tar - encs();

        i += error;
        d = error - prevError;

        P = error * driveP;
        I = i * driveI;
        D = d * driveD;

        dir = (error < 0) ? -1 : 1; // should be -1
        volt = abs(percent_speed*1.27)*dir;
        
        // limit max speed
        O = (abs(P + I + D) > abs(volt)) ? abs(volt)*dir : abs(P + I + D)*dir;
        printf("O2: %f\n", O);

        lPower = O;
        rPower = O;

        set_tank(lPower, rPower);
    }
    // Undershoot correction
     while ((encs() < tar - tolError)){
        prevError = error;
        error = tar - encs();

        i += error;
        d = error - prevError;

        P = error * driveP;
        I = i * driveI;
        D = d * driveD;

        dir = (error < 0) ? -1 : 1; // should be 1
        volt = abs(percent_speed*1.27)*dir;

        // limit max speed
        O = (abs(P + I + D) > abs(volt)) ? abs(volt)*dir : abs(P + I + D)*dir;
        printf("O3: %f\n", O);

        lPower = O;
        rPower = O;

        set_tank(lPower, rPower);
    }
    off();
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

    double O = 0;
    double Ol = 0;
    double Or = 0;

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

        // limit max speed
        Ol = (abs(P + I + D) > abs(volt)) ? abs(volt)*direction : abs(P + I + D)*direction;
        Ol = (abs(P + I + D) > abs(volt)) ? abs(volt)*-direction : abs(P + I + D)*-direction;

        lPower = Ol;
        rPower = Or;

        // lPower = (volt + P + I + D)*direction;
        // rPower = (volt + P + I + D)*-direction;
        
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

        direction *= -1;
        // lPower = (volt + P + I + D)*-direction;
        // rPower = (volt + P + I + D)*direction;

        // limit max speed
        Ol = (abs(P + I + D) > abs(volt)) ? abs(volt)*direction : abs(P + I + D)*direction;
        Ol = (abs(P + I + D) > abs(volt)) ? abs(volt)*-direction : abs(P + I + D)*-direction;

        lPower = Ol;
        rPower = Or;
        
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

        direction *= -1;
        // lPower = (volt + P + I + D)*direction;
        // rPower = (volt + P + I + D)*-direction;

        // limit max speed
        Ol = (abs(P + I + D) > abs(volt)) ? abs(volt)*direction : abs(P + I + D)*direction;
        Ol = (abs(P + I + D) > abs(volt)) ? abs(volt)*-direction : abs(P + I + D)*-direction;

        lPower = Ol;
        rPower = Or;
        
        set_tank(lPower, rPower);
    }
    off();
}

void Drive::settle_drive(){
    off();
    reset_drive_sensors();
    double prev = -10;
    double curr = encs();
    while(prev != curr){
        prev = curr;
        curr = encs();
        pros::delay(20);
    }
}