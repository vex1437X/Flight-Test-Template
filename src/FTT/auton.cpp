// adapted from https://github.com/EZ-Robotics/EZ-Template

#include "main.h"

Auton::Auton() {
  Name = "";
  auton_call = nullptr;
}
Auton::Auton(std::string name, std::function<void()> callback) {
  Name = name;
  auton_call = callback;
}
