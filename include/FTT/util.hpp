#pragma once

#include <bits/stdc++.h>
#include <stdio.h>
#include <string.h>

#include "api.h"

extern pros::Controller master;

namespace ftt { // creating the flight test template namespace
    /**
     * Returns true if the input is < 0
     */
    bool is_reversed(double input);

    void set_robot();

    /**
     * Displays temperature of all motors
     */
    void screen_checkup();
}

