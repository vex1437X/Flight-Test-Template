#include "main.h"
#include "FTT/skills.hpp"
#include "pros/motors.h"

using namespace ftt;

Skills::Skills(int gps_sensor_port){
    pros::GPS g(gps_sensor_port);
    gps.push_back(g);

}

void Skills::initialise(){

}