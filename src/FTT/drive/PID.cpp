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
int H;

bool test = false;

void Drive::auton_pid_task(){
    printf("v\n");
    while(enable_drive_pid){
        printf("driv\n");
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

            tolError = (drive_target/200) * TPI; // error tolerance

            headingInit = get_locked_heading();
            headingError = 0;
            H = 0;
            drive_init = true;
            
        }
        // tar = drive_target * TPI;
        prevError = error;
        error = tar - encs();
        headingError = headingInit - get_locked_heading(); // + --> right more power | - --> left more power
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
        if (x1>=30 && !drive_init){
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
        printf("tur\n");
        if (!turn_init){
            reset_drive_sensors();
            dir = turn_direction; // turning direction (+1 for clockwise; -1 for counterclockwise)
            printf("startDIR: %d \n", dir);
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

            double pre = (dir == -1 && get_locked_heading() < 0.25) ? 359.999 : get_locked_heading(); // used just for startlow calc

            startlow = pre < turn_target ? true : false;

            tolError = 0.1; // error tolerance
            test = false;
            turn_init = true;
        }

        prevError = error;
        error = turn_target - get_locked_heading();
        printf("Error: %f \n", error);
        printf("Heading: %f \n", get_locked_heading());
        printf("Target: %f \n", turn_target);

        printf("Direction: %d \n", dir);

        i += error;
        d = error - prevError;

        P = error * turnP;
        I = i * turnI;
        D = d * turnD;

        printf("P: %f \n", P);
        printf("I: %f\n", I);
        printf("D: %f\n", D);

        double pre = (dir == -1 && get_locked_heading() < 0.25) ? 359.999 : get_locked_heading(); // used just for startlow calc
        nowlow = pre < turn_target ? true : false;
        if (!test){
            if (nowlow != startlow) dir*=-1;
            test = true;
        }

        // limit max speed
        Ol = (abs(P + I + D) > abs(volt)) ? abs(volt) : P + I + D;
        Or = (abs(P + I + D) > abs(volt)) ? abs(volt) : P + I + D;
        // limit min speed
        Ol = (abs(Ol) < 4) ? 4 : Ol;
        Or = (abs(Or) < 4) ? 4 : Or;
        // printf("pow%f\n", Ol);
        // printf("OTleft: %f\n", Ol);
        // printf("OTright: %f\n", Or);

        printf("Direction: %d \n", dir);

        lPower = Ol;
        rPower = -Or;

        set_tank(lPower, rPower);
        
        // exit case
        while ((get_locked_heading() > turn_target - tolError && get_locked_heading() < turn_target + tolError) && x1 < 30){
            x1++;
            pros::delay(10);
        }
        if (x1>=30 && !turn_init){
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

void Drive::turn_pid(double target, double percent_speed, int direction){
    turn_target = target;
    turn_percent_speed = percent_speed;
    turn_direction = direction;
    enable_turn_pid = true;
}

void Drive::settle_drive(){
    int x = 0;
    double prev = encs()+10;
    double curr = encs();
    while(x < 100){
        prev = curr;
        curr = encs();
        if (prev == curr) x++;
        pros::delay(10);
    }
}