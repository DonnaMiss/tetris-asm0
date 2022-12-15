#include "openvinoInferEng.h"
#include "./openvino/common.hpp"

using namespace InferenceEngine;

static UNUSED InferenceEngine::Blob::Ptr wrapMat2Blob(const cv::M