#pragma once

#include "inferenceEngineTemplate.h"
#include "mxnet-cpp/MxNetCpp.h"
using namespace mxnet::cpp;

class InferenceEng : InferenceEngineTemplate {
public:
    InferenceEng(const std::string& modelDir);
    ~InferenceEng();
    void runInference(const cv::Mat& rgbImage, std::array<float, 500>& output) override;
private:
    inline NDArray matToNDArray(cv::Mat rgb_image, Context ctx = Context::cpu());
    std::map<std::str