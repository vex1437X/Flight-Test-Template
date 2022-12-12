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

double drive_target = 0;
double drive_percent_speed = 0;
double turn_target = 0;
double turn_percent_speed = 0;
int turn_direction = 0;
bool enable_drive_pid = false;
bool enable_turn_pid = false;
double volt = 0;
int x1 = 0;

bool drive_init = false;
bool turn_init = false;

int dir;
double TPI;
double tar;
double error;
double prevError;
double P;
double i;
double I;
double d;
double D;
double O;
double Ol;
double Or;
double lPower;
double rPower;
double tolError;
double headingInit;
double headingError;
int H;

void Drive::auton_pid_task(){
    reset_drive_sensors();
    x1 = 0;
    drive_init = false;
    turn_init = false;
    drive_target = 0;
    turn_target = 0;
    while(enable_drive_pid){
        if (!drive_init){
            dir = (drive_target < 0) ? -1: 1; // driving direction (+1 for forwards; -1 for backwards)
            volt = abs(drive_percent_speed*1.27)*dir; // percent to voltage

            // convert target inches into target encoder
            TPI = get_tick_per_inch();
            tar = drive_target * TPI; 

            error = 0;
            prevError = 0;
            P = 0;
            i = 0;
            I = 0;
            d = 0;
            D = 0;

            O = 0;
            Ol = 0;
            Or = 0;

            lPower = 0;
            rPower = 0;

            tolError = (drive_target/100) * TPI; // error tolerance

            headingInit = imu.get_heading();
            headingError = 0;
            H = 0;
            drive_init = true;
        }
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

        // limit max speed
        Ol = (abs(P + I + D - H) > abs(volt)) ? abs(volt)*dir : abs(P + I + D - H)*dir;
        Or = (abs(P + I + D + H) > abs(volt)) ? abs(volt)*dir : abs(P + I + D + H)*dir;

        lPower = Ol;
        rPower = Or;

        set_tank(lPower, rPower);
        
        // exit case
        while ((encs() > tar - tolError && encs() < tar + tolError) && x1 < 30){
            x1++;
            pros::delay(10);
        }
        if (x1>=30){
            x1 = 0;
            enable_drive_pid = false;
            drive_init = false;
        }
        x1 = 0;
        pros::delay(10);
    }
    while(enable_turn_pid){
        if (!turn_init){
            volt = turn_percent_speed*1.27; // percent to voltage

            error = 0;
            prevError = 0;
            P = 0;
            i = 0;
            I = 0;
            d = 0;
            D = 0;

            O = 0;
            Ol = 0;
            Or = 0;

            lPower = 0;
            rPower = 0;

            tolError = 0.05; // error tolerance
            turn_init = true;
        }

        prevError = error;
        error = turn_target - imu.get_heading();

        i += error;
        d = error - prevError;

        P = error * turnP;
        I = i * turnI;
        D = d * turnD;

        // limit max speed
        Ol = (abs(P + I + D) > abs(volt)) ? abs(volt)*turn_direction : abs(P + I + D)*turn_direction;
        Or = (abs(P + I + D) > abs(volt)) ? abs(volt)*-turn_direction : abs(P + I + D)*-turn_direction;

        lPower = Ol;
        rPower = Or;

        set_tank(lPower, rPower);
        
        // exit case
        while ((imu.get_heading() > turn_target - tolError && imu.get_heading() < turn_target + tolError) && x1 < 30){
            x1++;
            pros::delay(10);
        }
        if (x1>=30){
            x1 = 0;
            enable_turn_pid = false;
            turn_init = false;
        }
        x1 = 0;
        pros::delay(10);
    }
    pros::delay(10);
}

void Drive::drive_pid(double target, double percent_speed){
    drive_target = target;
    drive_percent_speed = percent_speed;
    enable_drive_pid = true;
}

void Drive::turn_pid(double target, double percent_speed, int direction){
    turn_target = target;
    turn_percent_speed = percent_speed;
    turn_direction = direction;
    enable_turn_pid = true;
}

void Drive::settle_drive(){
    reset_drive_sensors();
    int x = 0;
    double prev = -10;
    double curr = encs();
    while(prev != curr && x < 30){
        prev = curr;
        curr = encs();
        x++; 
        pros::delay(10);
    }
}