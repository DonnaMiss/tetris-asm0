#include <vector>
#include "onnxRuntimeDefaultInferEng.h"

InferenceEng::InferenceEng(const std::string &modelDir) {
    std::cout << "Using onnxruntime inference engine" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    m_envPtr = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "te