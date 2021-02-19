#pragma once
#include <memory>

#include "inferenceEngineTemplate.h"
#include "onnxruntime_cxx_api.h"

// Sample code here: https://github.com/microsoft/onnxruntime/blob/master/csharp/test/Microsoft.ML.OnnxRuntime.EndToEndTests.Capi/CXX_Api_Sample.cpp#L24-L30

class InferenceEng : InferenceEngineTemplate {
public:
    InferenceEng(const std::string& modelDir);
    ~InferenceEng() = default;
    void runInference(const cv::Mat& rgbImage, std::array<float, 