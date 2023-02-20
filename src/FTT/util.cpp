#include "main.h"

pros::Controller master(pros::E_CONTROLLER_MASTER);

bool intake = false;
bool colourwheel = false;
bool flywheel = false;
bool catapult = false;

int ftt_NUM_INTAKE = 0;
int ftt_NUM_COLOURWHEEL = 0;
int ftt_NUM_FLYWHEEL = 0;
int ftt_NUM_CATAPULT = 0;

namespace ftt { // creating the flight test template namespace
    bool is_reversed(double input) {
        if (input < 0) return true;
        return false;
    }

    void set_robot(){
        if (sys.contains_intake()) intake = true;
        if (sys.contains_colourwheel()) colourwheel = true;
        if (sys.contains_flywheel()) flywheel = true;
        if (sys.contains_catapult()) catapult = true;

        ftt_NUM_INTAKE = sys.NUM_INTAKE;
        ftt_NUM_COLOURWHEEL = sys.NUM_COLOURWHEEL;
        ftt_NUM_FLYWHEEL = sys.NUM_FLYWHEEL;
        ftt_NUM_CATAPULT = sys.NUM_CATAPULT;
    }

    void screen_checkup(){
        // get vector of temps in each motor system
        
    }
}