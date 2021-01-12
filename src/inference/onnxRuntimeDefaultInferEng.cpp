#include <vector>
#include "onnxRuntimeDefaultInferEng.h"

InferenceEng::InferenceEng(const std::string &modelDir) {
    std::cout << "Using onnxruntime inference engine" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    m_envPtr = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "test");
    m_options.SetInterOpNumThreads(1);

    // Sets graph optimization level
    // Available levels are
    // ORT_DISABLE_ALL -> To disable all optimizations
    // ORT_ENABLE_BASIC -> To enable basic optimizations (Such as redundant node removals)
    // ORT_ENABLE_EXTENDED -> To enable extended optimizations (Includes level 1 + more complex optimizations like node fusions)
    // ORT_ENABLE_ALL -> To Enable All possible opitmizations
    m_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

    const std::string modelPath = modelDir + MODEL_NAME;
    m_sessionPtr = std::make_unique<Ort::Session>(*m_envPtr, modelPath.c_str(), m_options);
}

inline std::vector<float> rgbImgToFloatArr(cv::Mat rgb_image) {
    std::vector<float> data_buffer;

    // hwc to chw conversion
    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < rgb_image.rows; ++i) {
            for (int j = 0; j < rgb_image.cols; ++j) {
                data_buffer.push_back(static_cast<float>(rgb_image.data[(i * rgb_image.cols + j) * 3 + c]));
            }
        }
    }

    return data_buffer;
}

void InferenceEng::runInference(const cv::Mat& 