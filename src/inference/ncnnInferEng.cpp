#include "ncnnInferEng.h"

InferenceEng::InferenceEng(const std::string &modelDir) {
    std::cout << "Using ncnn inference engine" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    const std::string paramFilepath = modelDir + PARAMS_NAME;
    const std::string weighsFilepath = modelDir + WEIGHTS_NAME;
    m_net.load_param(paramFilepath.c_str());
    m_net.load_model(weighsFilepath.c_str());

    // Read number of threads to use
    if(const char* envP = std::getenv("OMP_NUM_THREADS")) {
        std::stringstream strValue;
        strValue << envP;
        strValue >> m_numThreads;
    }
}

void Inference