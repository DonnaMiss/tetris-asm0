#pragma once

#include "inferenceEngineTemplate.h"
#include <inference_engine.hpp>

class InferenceEng: InferenceEngineTemplate {
public:
    InferenceEng(const std::string& modelDir);
    void runInference(const cv::Mat& rgbImage, std::array<f