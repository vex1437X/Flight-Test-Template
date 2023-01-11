#include "main.h"

pros::Controller master(pros::E_CONTROLLER_MASTER);

namespace ftt { // creating the flight test template namespace
    bool is_reversed(double input) {
        if (input < 0) return true;
        return false;
    }

    // adapted from https://github.com/EZ-Robotics/EZ-Template
    std::string get_last_word(std::string text) {
        std::string word = "";
        for (int i = text.length() - 1; i >= 0; i--) {
            if (text[i] != ' ') {
                word += text[i];
            } else {
                std::reverse(word.begin(), word.end());
                return word;
            }
        }
        std::reverse(word.begin(), word.end());
        return word;
    }

    // adapted from https://github.com/EZ-Robotics/EZ-Template
    std::string get_rest_of_the_word(std::string text, int position) {
        std::string word = "";
        for (int i = position; i < text.length(); i++) {
            if (text[i] != ' ' && text[i] != '\n') {
                word += text[i];
            } else {
                return word;
            }
        }
        return word;
    }

    // adapted from https://github.com/EZ-Robotics/EZ-Template
    void print_to_screen(std::string text, int line) {
    int CurrAutoLine = line;
    std::vector<string> texts = {};
    std::string temp = "";

    for (int i = 0; i < text.length(); i++) {
        if (text[i] != '\n' && temp.length() + 1 > 32) {
            auto last_word = get_last_word(temp);
            if (last_word == temp) {
                texts.push_back(temp);
                temp = text[i];
            } else {
                int size = last_word.length(); 
                auto rest_of_word = get_rest_of_the_word(text, i); 
                temp.erase(temp.length() - size, size);
                texts.push_back(temp);
                last_word += rest_of_word;
                i += rest_of_word.length();
                temp = last_word;
                if (i >= text.length() - 1) {
                    texts.push_back(temp);
                break;
                }
            }
        }
        if (i >= text.length() - 1) {
            temp += text[i];
            texts.push_back(temp);
            temp = "";
            break;
        } else if (text[i] == '\n') {
            texts.push_back(temp);
            temp = "";
        } else {
            temp += text[i];
        }
    }
        for (auto i : texts) {
            if (CurrAutoLine > 7) {
                pros::lcd::clear();
                pros::lcd::set_text(line, "Out of Bounds. Print Line is too far down");
                return;
            }
            pros::lcd::clear_line(CurrAutoLine);
            pros::lcd::set_text(CurrAutoLine, i);
            CurrAutoLine++;
        }
    }
}