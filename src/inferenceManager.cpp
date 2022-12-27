
#include <opencv2/opencv.hpp>
#include <utility>
#include <array>
#include <chrono>
#include <fstream>
#include <math.h>

typedef std::chrono::high_resolution_clock Clock;

#include "inferenceManager.h"
#include "util.h"

InferenceManager::InferenceManager(const std::string &modelDir) {
    m_inferenceEnginePtr = std::make_unique<InferenceEng>(modelDir);
}

void InferenceManager::normalize(std::array<float, 500>& v) {
    auto mag = sqrt(dotProduct(v, v));
    for (auto& elem: v) {
        elem = elem / mag;
    }
}

float InferenceManager::dotProduct(const std::array<float, 500>& v1, const std::array<float, 500>& v2) {
    float dp = 0;
    for (size_t i = 0; i < 500; ++i) {
        dp += v1[i] * v2[i];
    }

    return dp;
}

void InferenceManager::readTemplateFromDisk(const std::string& templatePath, std::array<float, 500>& templ) {
    std::ifstream infile(templatePath, std::ifstream::binary);
    infile.read(reinterpret_cast<char*>(templ.data()), 2000);
}