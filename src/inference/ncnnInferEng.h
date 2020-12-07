#pragma once

#include "inferenceEngineTemplate.h"
#include "net.h"

class InferenceEng : InferenceEngineTemplate {
public:
    InferenceEng(const std::string& modelDir);
    ~InferenceEng() = default;
    void runInference(const cv::Mat& rgbImage, std::array<float, 500>& output) override;
private:
    ncnn::Net 