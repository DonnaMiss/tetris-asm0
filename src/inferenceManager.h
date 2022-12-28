#pragma once

#include <memory>


#ifdef USE_MXNET
#include "mxnetInferEng.h"
#elif USE_ONNX_DEFAULT
#inc