#include "main.h"
#include "FTT/drive/drive.hpp"

using namespace ftt;

void Drive::set_drive_pid(double p, double i, double d, double heading){
    driveP = p;
    driveI = i;
    driveD = d;
    driveH = heading;
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
bool settle = false;

bool startlow;
bool nowlow;

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
double H;

bool test = false;

void Drive::auton_pid_task(){
    while(enable_drive_pid){
        if (!drive_init){
            reset_drive_sensors();
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

            tolError = 100; // error tolerance

            headingInit = get_locked_heading();
            headingError = 0;
            H = 0;
            drive_init = true;
            settle = false;
        }
        // tar = drive_target * TPI;
        prevError = error;
        error = tar - encs();
        headingError = headingInit - get_locked_heading(); // + --> right more power | - --> left more power
        if (headingError < -180){headingError += 360;}
        else if(headingError > 180){headingError -= 360;}
        
        dir = (error < 0) ? -1: 1;

        i += error;
        d = error - prevError;

        P = error * driveP;
        I = i * driveI;
        D = d * driveD;
        H = headingError * driveH;

        // limit max speed
        Ol = (abs(P + I + D + H) > abs(volt)) ? abs(volt + H) : abs(P + I + D + H);
        Or = (abs(P + I + D - H) > abs(volt)) ? abs(volt - H) : abs(P + I + D - H);

        lPower = Ol * dir;
        rPower = Or * dir;

        set_tank(lPower, rPower);
        
        // exit case
        while ((encs() > tar - tolError && encs() < tar + tolError) && x1 < 30){
            x1++;
            pros::delay(10);
        }

        if (x1 >= 30 && settle){
            off();
            x1 = 0;
            enable_drive_pid = false;
            drive_init = false;
            drive_target = 0;
        }
        x1 = 0;
        pros::delay(10);
    }
    while(enable_turn_pid){
        if (!turn_init){
            reset_drive_sensors();
            settle = false;
            dir = ((int(turn_target) - int(get_locked_heading()) + 360) % 360 < 180) ? 1 : -1; 
                                        // turning direction (+1 for clockwise; -1 for counterclockwise)
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

            tolError = 0.1; // error tolerance
            test = false;
            turn_init = true;
        }

        prevError = error;
        error = turn_target - get_locked_heading();

        i += error;
        d = error - prevError;

        P = error * turnP;
        I = i * turnI;
        D = d * turnD;

        // limit max speed
        Ol = (abs(P + I + D) > abs(volt)) ? abs(volt) : P + I + D;
        Or = (abs(P + I + D) > abs(volt)) ? abs(volt) : P + I + D;
        // limit min speed
        Ol = (abs(Ol) < 4) ? 4 : Ol;
        Or = (abs(Or) < 4) ? 4 : Or;

        lPower = Ol * dir;
        rPower = -Or * dir;

        set_tank(lPower, rPower);
        
        // exit case
        while ((get_locked_heading() > turn_target - tolError && get_locked_heading() < turn_target + tolError) && x1 < 30){
            x1++;
            pros::delay(10);
        }
        if (x1>=30){
            off();
            x1 = 0;
            enable_turn_pid = false;
            turn_init = false;
            turn_target = 0;
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

void Drive::turn_pid(double target, double percent_speed){
    turn_target = target;
    turn_percent_speed = percent_speed;
    enable_turn_pid = true;
}

void Drive::settle_drive(){
    settle = false;
    int x = 0;
    double prev = encs()+10000;
    double curr = encs();
    while(x < 60){
        prev = curr;
        curr = encs();
        // if (curr == prev) x++;
        if (curr > prev - 10 && curr < prev + 10) x++;
        pros::delay(10);
    }
    settle = true;
}