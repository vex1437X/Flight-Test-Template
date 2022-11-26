// adapted from https://github.com/EZ-Robotics/EZ-Template

#include <filesystem>

#include "main.h"

namespace ftt { namespace as {
    AutonSelector auton_selector{};

    void update_auto_sd() {
        FILE* usd_file_write = fopen("/usd/auto.txt", "w");
        std::string cp_str = std::to_string(auton_selector.current_auton_page);
        char const* cp_c = cp_str.c_str();
        fputs(cp_c, usd_file_write);
        fclose(usd_file_write);
    }

    void init_auton_selector() {

    FILE* as_usd_file_read;
        // If file exists...
        if ((as_usd_file_read = fopen("/usd/auto.txt", "r"))) {
            char l_buf[5];
            fread(l_buf, 1, 5, as_usd_file_read);
            ftt::as::auton_selector.current_auton_page = std::stof(l_buf);
            fclose(as_usd_file_read);
        }
        // If file doesn't exist, create file
        else {
            update_auto_sd();  // Writing to a file that doesn't exist creates the file
            printf("Created auto.txt\n");
        }

        if (ftt::as::auton_selector.current_auton_page > ftt::as::auton_selector.auton_count - 1 || ftt::as::auton_selector.current_auton_page < 0) {
            ftt::as::auton_selector.current_auton_page = 0;
            ftt::as::update_auto_sd();
        }
    }

    void page_up() {
        if (auton_selector.current_auton_page == auton_selector.auton_count - 1)
            auton_selector.current_auton_page = 0;
        else
            auton_selector.current_auton_page++;
        update_auto_sd();
        auton_selector.print_selected_auton();
    }

    void page_down() {
        if (auton_selector.current_auton_page == 0)
            auton_selector.current_auton_page = auton_selector.auton_count - 1;
        else
            auton_selector.current_auton_page--;
        update_auto_sd();
        auton_selector.print_selected_auton();
    }

    void initialise() {
        // Initialize auto selector and LLEMU
        pros::lcd::initialize();
        ftt::as::init_auton_selector();

        // Callbacks for auto selector
        ftt::as::auton_selector.print_selected_auton();
        pros::lcd::register_btn0_cb(ftt::as::page_down);
        pros::lcd::register_btn2_cb(ftt::as::page_up);
    }

    void shutdown() {
        pros::lcd::shutdown();
    }

    bool turn_off = false;
} }
