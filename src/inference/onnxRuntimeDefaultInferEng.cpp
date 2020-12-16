#include <vector>
#include "onnxRuntimeDefaultInferEng.h"

InferenceEng::InferenceEng(const std::string &modelDir) {
    std::cout << "Using onnxruntime inference engine" << std::endl;
   