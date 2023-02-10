#pragma once

#include <functional>
#include <iostream>
#include <tuple>

// #include "FTT/PID.hpp"
#include "FTT/util.hpp"
#include "pros/adi.hpp"
#include "pros/motors.h"

using namespace ftt;

class Skills {
 public:

    /**
     * @brief Skills Constructor
     */
    Skills(int gps_sensor_port);

    /**
     * @brief Make GPS a global variable
     * 
     */
    std::vector<pros::GPS> gps;

    /**
     * @brief Initialise all the sensors in Skills
     * 
     */
    void initialise();
};