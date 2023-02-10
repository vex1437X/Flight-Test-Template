// adapted from https://github.com/EZ-Robotics/EZ-Template

#pragma once
#include <tuple>

#include "FTT/auton.hpp"

using namespace std;
class AutonSelector {
 public:
  std::vector<Auton> Autons;
  int current_auton_page;
  int auton_count;
  AutonSelector();
  AutonSelector(std::vector<Auton> autons);
  void call_selected_auton();
  void print_selected_auton();
  void add_autons(std::vector<Auton> autons);
  void initialise();
  void run();
};
