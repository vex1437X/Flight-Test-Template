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

    // adapted from https://github.com/EZ-Robotics/EZ-Template
    /**
     * Prints to the brain screen in one string.  Splits input between lines with
     * '\n' or when text longer then 32 characters.
     *
     * @param text
     *        Input string.  Use '\n' for a new line
     * @param line
     *        Starting line to print on, defaults to 0
     */
    void print_to_screen(std::string text, int line = 0);
}

