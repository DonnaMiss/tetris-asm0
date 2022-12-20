#include "openvinoInferEng.h"
#include "./openvino/common.hpp"

using namespace InferenceEngine;

static UNUSED InferenceEngine::Blob::Ptr wrapMat2Blob(const cv::Mat &mat) {
    size_t channels = mat.channels();
    size_t height = mat.size().height;
    size_t width = mat.size().width;

    size_t strideH = mat.step.buf[0];
    size_t strideW = mat.step.buf[1];

    bool is_dense =
            strideW == channels &&
            strideH == channels * width;

    if (!is_dense) THROW_IE_EXCEPTION
                << "Doesn't support conversion from not dense cv::Mat";

    InferenceEngine::TensorDesc tDesc(InferenceEngine::Precision::U8,
                                      {1, channels, height, width},
                                      InferenceEngine::Layout::NHWC);

    return InferenceEngine::make_shared_blob<uint8_t>(tDesc, mat.data);
}

InferenceEng::InferenceEng(const std::string &modelDir) {
    std::cout << "Using openvino inference engine" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    const std::string xmlPath = modelDir + "/" + XML_NAME;

    Core ie;
    CNNNetwork network = ie.ReadNetwork(xmlPath);

    InputInfo::Ptr inputInfo = network.getInputsInfo().begin()->s