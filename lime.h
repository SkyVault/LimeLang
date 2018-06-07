#pragma once
#define MAJOR_VERSION "0"
#define MINOR_VERSION "0"
#define REVISION_VERISON "0"

#define LIME_VERSION MAJOR_VERSION "." MINOR_VERSION "." REVISION_VERISON 

#define COLORED_OUTPUT

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>

enum class ansi_color_code: int{
    black = 30,
    red=31,
    green=32,
    yellow=33,
    blue=34,
    magenta=35,
    cyan=36,
    white=37,
    bright_black = 90,
    bright_red=91,
    bright_green=92,
    bright_yellow=93,
    bright_blue=94,
    bright_magenta=95,
    bright_cyan=96,
    bright_white=97,
};

template<typename printable>
static void print_as_color(printable const& value, ansi_color_code color){
    std::cout << "\033[1;" << std::to_string((int)color) << "m" << value <<"\033[0m" << std::endl;
}

void Error(std::string message, int line_number);

const std::string char_set = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789";

std::string gen_uuid(int num = 6);
