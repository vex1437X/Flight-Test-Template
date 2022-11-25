#include "main.h"

pros::Controller master(pros::E_CONTROLLER_MASTER);

namespace ftt { // creating the flight test template namespace
    bool is_reversed(double input) {
    if (input < 0) return true;
    return false;
    }
}