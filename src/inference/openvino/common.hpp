// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

/**
 * @brief a header file with common samples functionality
 * @file common.hpp
 */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <list>
#include <limits>
#include <functional>
#include <fstream>
#include <iomanip>
#include <utility>
#include <algorithm>
#include <random>

#include <inference_engine.hpp>

#ifndef UNUSED
  #if defined (_MSC_VER) && !defined (__clang__)
    #define UNUSED
  #else
    #define UNUSED  __attribute__((unused))
  #endif
#endif

/**
 * @brief trim from start (in place)
 * @param s - string to trim
 */
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c){
        return !std::isspace(c);
    }));
}

/**
 * @brief trim from end (in place)
 * @param s - string to trim
 */
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {
        return !std::isspace(c);
    }).base(), s.end());
}

/**
 * @brief trim from both ends (in place)
 * @param s - string to trim
 */
inline std::string &trim(std::string &s) {
    ltrim(s);
    rtrim(s);
    return s;
}
/**
 * @brief Gets filename without extension
 * @param filepath - full file name
 * @return filename without extension
 */
static UNUSED std::string fileNameNoExt(const std::string &filepath) {
    auto pos = filepath.rfind('.');
    if (pos == std::string::npos) return filepath;
    return filepath.substr(0, pos);
}

/**
* @brief Get extension from filename
* @param filename - name of the file which extension should be extracted
* @return string with extracted file extension
*/
inline std::string fileExt(const std::string& filename) {
    auto pos = filename.rfind('.');
    if (pos == std::string::npos) return "";
    return filename.substr(pos + 1);
}

static UNUSED std::ostream &operator<<(std::ostream &os, const InferenceEngine::Version *version) {
    os << "\n\tAPI version ............ ";
    if (nullptr == version) {
        os << "UNKNOWN";
    } else {
        os << version->apiVersion.major << "." << version->apiVersion.minor;
        if (nullptr != version->buildNumber) {
            os << "\n\t" << "Build .................. " << version->buildNumber;
        }
        if (nullptr != version->description) {
            os << "\n\t" << "Description ....... " << version->description;
        }
    }
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const InferenceEngine::Version &version) {
    os << "\t" << version.description << " version ......... ";
    os << version.apiVersion.major << "." << version.apiVersion.minor;

    os << "\n\tBuild ........... ";
    os << version.buildNumber;

    return os;
}

inline std::ostream &operator<<(std::ostream &os, const std::map<std::string, InferenceEngine::Version> &versions) {
    for (auto && version : versions) {
        os << "\t" << version.first << std::endl;
        os << version.second << std::endl;
    }

    return os;
}

static UNUSED std::vector<std::vector<size_t>> blobToImageOutputArray(InferenceEngine::TBlob<float>::Ptr output,
                                                                      size_t *pWidth, size_t *pHeight,
                                                                      size_t *pChannels) {
    std::vector<std::vector<size_t>> outArray;
    size_t W = 0, C = 0, H = 0;

    auto outputDims = output->getTensorDesc().getDims();
    if (outputDims.size() == 3) {
        C = outputDims.at(0);
        H = outputDims.at(1);
        W = outputDims.at(2);
    } else if (outputDims.size() == 4) {
        C = outputDims.at(1);
        H = outputDims.at(2);
        W = outputDims.at(3);
    } else if (outputDims.size() == 5) {
        C = outputDims.at(1);
        H = outputDims.at(3);
        W = outputDims.at(4);
    } else {
        THROW_IE_EXCEPTION << "Output blob has unsupported layout " << output->getTensorDesc().getLayout();
    }

    // Get classes
    const float *outData = output->data();
    for (unsigned h = 0; h < H; h++) {
        std::vector<size_t> row;
        for (unsigned w = 0; w < W; w++) {
            float max_value = outData[h * W + w];
            size_t index = 0;
            for (size_t c = 1; c < C; c++) {
                size_t dataIndex = c * H * W + h * W + w;
                if (outData[dataIndex] > max_value) {
                    index = c;
                    max_value = outData[dataIndex];
                }
            }
            row.push_back(index);
        }
        outArray.push_back(row);
    }

    if (pWidth != nullptr) *pWidth = W;
    if (pHeight != nullptr) *pHeight = H;
    if (pChannels != nullptr) *pChannels = C;

    return outArray;
}

/**
 * @class Color
 * @brief A Color class stores channels of a given color
 */
class Color {
private:
    unsigned char _r;
    unsigned char _g;
    unsigned char _b;

public:
    /**
     * A default constructor.
     * @param r - value for red channel
     * @param g - value for green channel
     * @param b - value for blue channel
     */
    Color(unsigned char r,
          unsigned char g,
          unsigned char b) : _r(r), _g(g), _b(b) {}

    inline unsigned char red() {
        return _r;
    }

    inline unsigned char blue() {
        return _b;
    }

    inline unsigned char green() {
        return _g;
    }
};

// TODO : keep only one version of writeOutputBMP

/**
 * @brief Writes output data to image
 * @param name - image name
 * @param data - output data
 * @param classesNum - the number of classes
 * @return false if error else true
 */
static UNUSED void writeOutputBmp(std::vector<std::vector<size_t>> data, size_t classesNum, std::ostream &outFile) {
    unsigned int seed = (unsigned int) time(NULL);
    // Known colors for training classes from Cityscape dataset
    static std::vector<Color> colors = {
        {128, 64,  128},
        {232, 35,  244},
        {70,  70,  70},
        {156, 102, 102},
        {153, 153, 190},
        {153, 153, 153},
        {30,  170, 250},
        {0,   220, 220},
        {35,  142, 107},
        {152, 251, 152},
        {180, 130, 70},
        {60,  20,  220},
        {0,   0,   255},
        {142, 0,   0},
        {70,  0,   0},
        {100, 60,  0},
        {90,  0,   0},
        {230, 0,   0},
        {32,  11,  119},
        {0,   74,  111},
        {81,  0,   81}
    };

    while (classesNum > colors.size()) {
        static std::mt19937 rng(seed);
        std::uniform_int_distribution<int> dist(0, 255);
        Color color(dist(rng), dist(rng), dist(rng));
        colors.push_back(color);
    }

    unsigned char file[14] = {
            'B', 'M',           // magic
            0, 0, 0, 0,         // size in bytes
            0, 0,               // app data
            0, 0,             