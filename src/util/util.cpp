#include <unistd.h>
#include <ios>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <array>
#include <memory>
#include "util.h"

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std: