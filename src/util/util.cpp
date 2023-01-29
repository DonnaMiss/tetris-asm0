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
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

unsigned int getNumThreads() {
    pid_t pid = getpid();
    std::string command = "cat /proc/" + std::to_string(pid) + "/status | g