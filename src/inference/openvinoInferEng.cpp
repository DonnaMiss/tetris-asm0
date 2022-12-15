#include "openvinoInferEng.h"
#include "./openvino/common.hpp"

using namespace InferenceEngine;

static UNUSED InferenceEngine::Blob::Ptr wrapMat2Blob(const cv::Mat &mat) {
    size_t channels = mat.channels();
    size_t height = mat.size().height;
    siz