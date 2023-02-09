// adapted from https://github.com/EZ-Robotics/EZ-Template

#include "main.h"
#include "pros/misc.h"
#include "pros/screen.h"
#include "pros/screen.hpp"

#define BRAIN_WIDTH 480
#define BRAIN_HEIGHT 272

AutonSelector::AutonSelector() {
  auton_count = 0;
  current_auton_page = 0;
  Autons = {};
}

AutonSelector::AutonSelector(std::vector<Auton> autons) {
  auton_count = autons.size();
  current_auton_page = 0;
  Autons = {};
  Autons.assign(autons.begin(), autons.end());
}

void AutonSelector::print_selected_auton() {
  // if (auton_count == 0) return;
  // for (int i = 0; i < 8; i++)
  //   pros::lcd::clear_line(i);
  // ftt::print_to_screen("Page " + std::to_string(current_auton_page + 1) + "\n" + Autons[current_auton_page].Name);
}

void AutonSelector::call_selected_auton() {
  if (auton_count == 0) return;
  Autons[current_auton_page].auton_call();
}

void AutonSelector::add_autons(std::vector<Auton> autons) {
  auton_count += autons.size();
  current_auton_page = 0;
  Autons.assign(autons.begin(), autons.end());
}

void AutonSelector::initialise(){
  pros::screen::set_pen(COLOR_BLACK);
  pros::screen::draw_rect(0, 0, BRAIN_WIDTH, BRAIN_HEIGHT);
  pros::screen::fill_rect(0, 0, BRAIN_WIDTH, BRAIN_HEIGHT);
  if (auton_count != 0){
    pros::screen::set_pen(COLOR_BLUE_VIOLET);
    pros::c::screen_set_pen(COLOR_BLUE_VIOLET);
    const char * c = Autons[current_auton_page].Name.c_str();
    pros::c::screen_print_at(pros::E_TEXT_LARGE, BRAIN_WIDTH/4, BRAIN_HEIGHT/2, c);
  }
  while (true){
    if (master.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT)){
      if (current_auton_page > 0){
        current_auton_page--;
        pros::screen::set_pen(COLOR_BLACK);
        pros::screen::draw_rect(0, 0, BRAIN_WIDTH, BRAIN_HEIGHT);
        pros::screen::fill_rect(0, 0, BRAIN_WIDTH, BRAIN_HEIGHT);
        pros::screen::set_pen(COLOR_BLUE_VIOLET);
        pros::c::screen_set_pen(COLOR_BLUE_VIOLET);
        const char * c = Autons[current_auton_page].Name.c_str();
        pros::c::screen_print_at(pros::E_TEXT_LARGE, BRAIN_WIDTH/4, BRAIN_HEIGHT/2, c);
      }
      pros::delay(350);
    }
    if (master.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT)){
      if((current_auton_page + 1) < auton_count){
        current_auton_page--;
        pros::screen::set_pen(COLOR_BLACK);
        pros::screen::draw_rect(0, 0, BRAIN_WIDTH, BRAIN_HEIGHT);
        pros::screen::fill_rect(0, 0, BRAIN_WIDTH, BRAIN_HEIGHT);
        pros::screen::set_pen(COLOR_BLUE_VIOLET);
        pros::c::screen_set_pen(COLOR_BLUE_VIOLET);
        const char * c = Autons[current_auton_page].Name.c_str();
        pros::c::screen_print_at(pros::E_TEXT_LARGE, BRAIN_WIDTH/4, BRAIN_HEIGHT/2, c);
      }
      pros::delay(350);
    }
    pros::delay(10);
  }
}