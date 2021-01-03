#include <vector>
#include "onnxRuntimeDefaultInferEng.h"

InferenceEng::InferenceEng(const std::string &modelDir) {
    std::cout << "Using onnxruntime inference engine" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    m_envPtr = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "test");
    m_options.SetInterOpNumThreads(1);

    // Sets graph opt