#pragma once

#include "inferenceEngineTemplate.h"
#include "mxnet-cpp/MxNetCpp.h"
using namespace mxnet::cpp;

class InferenceEng : InferenceEngineTemplate {
public:
    InferenceEng(const std::string& modelDir);
    ~InferenceEng();
    void runInference(const cv::Mat& rgbImage, std::array<fl