#pragma once

#include <memory>


#ifdef USE_MXNET
#include "mxnetInferEng.h"
#elif USE_ONNX_DEFAULT
#include "onnxRuntimeDefaultInferEng.h"
#elif USE_OPENVINO
#include "openvinoInferEng.h"
#else
#include "ncnnInferEng.h"
#endif

class InferenceManager {
public:
    InferenceManager(const std::string& modelDir);

    void runBenchmark(unsigned int numIterations = 1000);
private:
    std::unique_ptr<InferenceEng> m_inferenceEnginePtr = n