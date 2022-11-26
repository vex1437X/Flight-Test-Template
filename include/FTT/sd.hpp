// adapted from https://github.com/EZ-Robotics/EZ-Template

#pragma once

#include "FTT/auton_selector.hpp"
#include "api.h"

namespace ftt {
namespace as {
    extern AutonSelector auton_selector;
    /**
     * Sets sd card to current page.
     */
    void init_auton_selector();

    /**
     * Sets the sd card to current page.
     */
    void update_auto_sd();

    /**
     * Increases the page by 1.
     */
    void page_up();

    /**
     * Decreases the page by 1.
     */
    void page_down();

    /**
     * Initializes LLEMU and sets up callbacks for auton selector.
     */
    void initialise();

    /**
     * Wrapper for pros::lcd::shutdown.
     */
    void shutdown();

    extern bool turn_off;
} }
