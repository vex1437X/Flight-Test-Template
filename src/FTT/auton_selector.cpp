// adapted from https://github.com/EZ-Robotics/EZ-Template

#include "main.h"
#include "pros/misc.h"
#include "pros/screen.h"
#include "pros/screen.hpp"
#include "FTT/drive/drive.hpp"
using namespace ftt;


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
    pros::c::screen_print_at(pros::E_TEXT_LARGE, BRAIN_WIDTH/3, BRAIN_HEIGHT/3, c);
  }
}

void AutonSelector::run(){
  while (true){
    if (master.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT) || master.get_digital(pros::E_CONTROLLER_DIGITAL_UP)){
      if (current_auton_page > 0){
        current_auton_page--;
        pros::screen::set_pen(COLOR_BLACK);
        pros::screen::draw_rect(0, 0, BRAIN_WIDTH, BRAIN_HEIGHT);
        pros::screen::fill_rect(0, 0, BRAIN_WIDTH, BRAIN_HEIGHT);
        pros::screen::set_pen(COLOR_BLUE_VIOLET);
        pros::c::screen_set_pen(COLOR_BLUE_VIOLET);
        const char * d = Autons[current_auton_page].Name.c_str();
        pros::c::screen_print_at(pros::E_TEXT_LARGE, BRAIN_WIDTH/3, BRAIN_HEIGHT/3, d);
      }
      pros::delay(350);
    }
    if (master.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT) || master.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN)){
      if((current_auton_page + 1) < auton_count){
        current_auton_page++;
        pros::screen::set_pen(COLOR_BLACK);
        pros::screen::draw_rect(0, 0, BRAIN_WIDTH, BRAIN_HEIGHT);
        pros::screen::fill_rect(0, 0, BRAIN_WIDTH, BRAIN_HEIGHT);
        pros::screen::set_pen(COLOR_BLUE_VIOLET);
        pros::c::screen_set_pen(COLOR_BLUE_VIOLET);
        const char * f = Autons[current_auton_page].Name.c_str();
        pros::c::screen_print_at(pros::E_TEXT_LARGE, BRAIN_WIDTH/3, BRAIN_HEIGHT/3, f);
      }
      pros::delay(350);
    }
    if (master.get_digital(pros::E_CONTROLLER_DIGITAL_X) || master.get_digital(pros::E_CONTROLLER_DIGITAL_Y)
         || master.get_digital(pros::E_CONTROLLER_DIGITAL_A) || master.get_digital(pros::E_CONTROLLER_DIGITAL_B)){
      pros::screen::set_pen(COLOR_BLACK);
      pros::screen::draw_rect(0, 0, BRAIN_WIDTH, BRAIN_HEIGHT);
      pros::screen::fill_rect(0, 0, BRAIN_WIDTH, BRAIN_HEIGHT);
      pros::screen::set_pen(COLOR_GREEN);
      pros::c::screen_set_pen(COLOR_GREEN);
      const char * f = Autons[current_auton_page].Name.c_str();
      pros::c::screen_print_at(pros::E_TEXT_LARGE, BRAIN_WIDTH/3, BRAIN_HEIGHT/3, f, " has been selected");
      return;
    }
    pros::delay(10);
  }
}