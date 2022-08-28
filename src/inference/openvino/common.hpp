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
            0, 0,               // app data
            40 + 14, 0, 0, 0      // start of data offset
    };
    unsigned char info[40] = {
            40, 0, 0, 0,        // info hd size
            0, 0, 0, 0,         // width
            0, 0, 0, 0,         // height
            1, 0,               // number color planes
            24, 0,              // bits per pixel
            0, 0, 0, 0,         // compression is none
            0, 0, 0, 0,         // image bits size
            0x13, 0x0B, 0, 0,   // horz resolution in pixel / m
            0x13, 0x0B, 0, 0,   // vert resolution (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
            0, 0, 0, 0,         // #colors in palette
            0, 0, 0, 0,         // #important colors
    };

    auto height = data.size();
    auto width = data.at(0).size();

    if (height > (size_t) std::numeric_limits<int32_t>::max || width > (size_t) std::numeric_limits<int32_t>::max) {
        THROW_IE_EXCEPTION << "File size is too big: " << height << " X " << width;
    }

    int padSize = static_cast<int>(4 - (width * 3) % 4) % 4;
    int sizeData = static_cast<int>(width * height * 3 + height * padSize);
    int sizeAll = sizeData + sizeof(file) + sizeof(info);

    file[2] = (unsigned char) (sizeAll);
    file[3] = (unsigned char) (sizeAll >> 8);
    file[4] = (unsigned char) (sizeAll >> 16);
    file[5] = (unsigned char) (sizeAll >> 24);

    info[4] = (unsigned char) (width);
    info[5] = (unsigned char) (width >> 8);
    info[6] = (unsigned char) (width >> 16);
    info[7] = (unsigned char) (width >> 24);

    int32_t negativeHeight = -(int32_t) height;
    info[8] = (unsigned char) (negativeHeight);
    info[9] = (unsigned char) (negativeHeight >> 8);
    info[10] = (unsigned char) (negativeHeight >> 16);
    info[11] = (unsigned char) (negativeHeight >> 24);

    info[20] = (unsigned char) (sizeData);
    info[21] = (unsigned char) (sizeData >> 8);
    info[22] = (unsigned char) (sizeData >> 16);
    info[23] = (unsigned char) (sizeData >> 24);

    outFile.write(reinterpret_cast<char *>(file), sizeof(file));
    outFile.write(reinterpret_cast<char *>(info), sizeof(info));

    unsigned char pad[3] = {0, 0, 0};

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            unsigned char pixel[3];
            size_t index = data.at(y).at(x);
            pixel[0] = colors.at(index).red();
            pixel[1] = colors.at(index).green();
            pixel[2] = colors.at(index).blue();
            outFile.write(reinterpret_cast<char *>(pixel), 3);
        }
        outFile.write(reinterpret_cast<char *>(pad), padSize);
    }
}

/**
* @brief Writes output data to BMP image
* @param name - image name
* @param data - output data
* @param height - height of the target image
* @param width - width of the target image
* @return false if error else true
*/
static UNUSED bool writeOutputBmp(std::string name, unsigned char *data, size_t height, size_t width) {
    std::ofstream outFile;
    outFile.open(name, std::ofstream::binary);
    if (!outFile.is_open()) {
        return false;
    }

    unsigned char file[14] = {
        'B', 'M',           // magic
        0, 0, 0, 0,         // size in bytes
        0, 0,               // app data
        0, 0,               // app data
        40 + 14, 0, 0, 0      // start of data offset
    };
    unsigned char info[40] = {
        40, 0, 0, 0,        // info hd size
        0, 0, 0, 0,         // width
        0, 0, 0, 0,         // height
        1, 0,               // number color planes
        24, 0,              // bits per pixel
        0, 0, 0, 0,         // compression is none
        0, 0, 0, 0,         // image bits size
        0x13, 0x0B, 0, 0,   // horz resolution in pixel / m
        0x13, 0x0B, 0, 0,   // vert resolution (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
        0, 0, 0, 0,         // #colors in palette
        0, 0, 0, 0,         // #important colors
    };

    if (height > (size_t)std::numeric_limits<int32_t>::max || width > (size_t)std::numeric_limits<int32_t>::max) {
        THROW_IE_EXCEPTION << "File size is too big: " << height << " X " << width;
    }

    int padSize = static_cast<int>(4 - (width * 3) % 4) % 4;
    int sizeData = static_cast<int>(width * height * 3 + height * padSize);
    int sizeAll = sizeData + sizeof(file) + sizeof(info);

    file[2] = (unsigned char)(sizeAll);
    file[3] = (unsigned char)(sizeAll >> 8);
    file[4] = (unsigned char)(sizeAll >> 16);
    file[5] = (unsigned char)(sizeAll >> 24);

    info[4] = (unsigned char)(width);
    info[5] = (unsigned char)(width >> 8);
    info[6] = (unsigned char)(width >> 16);
    info[7] = (unsigned char)(width >> 24);

    int32_t negativeHeight = -(int32_t)height;
    info[8] = (unsigned char)(negativeHeight);
    info[9] = (unsigned char)(negativeHeight >> 8);
    info[10] = (unsigned char)(negativeHeight >> 16);
    info[11] = (unsigned char)(negativeHeight >> 24);

    info[20] = (unsigned char)(sizeData);
    info[21] = (unsigned char)(sizeData >> 8);
    info[22] = (unsigned char)(sizeData >> 16);
    info[23] = (unsigned char)(sizeData >> 24);

    outFile.write(reinterpret_cast<char *>(file), sizeof(file));
    outFile.write(reinterpret_cast<char *>(info), sizeof(info));

    unsigned char pad[3] = { 0, 0, 0 };

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            unsigned char pixel[3];
            pixel[0] = data[y * width * 3 + x * 3];
            pixel[1] = data[y * width * 3 + x * 3 + 1];
            pixel[2] = data[y * width * 3 + x * 3 + 2];

            outFile.write(reinterpret_cast<char *>(pixel), 3);
        }
        outFile.write(reinterpret_cast<char *>(pad), padSize);
    }
    return true;
}


/**
* @brief Adds colored rectangles to the image
* @param data - data where rectangles are put
* @param height - height of the rectangle
* @param width - width of the rectangle
* @param rectangles - vector points for the rectangle, should be 4x compared to num classes
* @param classes - vector of classes
* @param thickness - thickness of a line (in pixels) to be used for bounding boxes
*/
static UNUSED void addRectangles(unsigned char *data, size_t height, size_t width, std::vector<int> rectangles, std::vector<int> classes, int thickness = 1) {
    std::vector<Color> colors = {  // colors to be used for bounding boxes
        { 128, 64,  128 },
        { 232, 35,  244 },
        { 70,  70,  70 },
        { 156, 102, 102 },
        { 153, 153, 190 },
        { 153, 153, 153 },
        { 30,  170, 250 },
        { 0,   220, 220 },
        { 35,  142, 107 },
        { 152, 251, 152 },
        { 180, 130, 70 },
        { 60,  20,  220 },
        { 0,   0,   255 },
        { 142, 0,   0 },
        { 70,  0,   0 },
        { 100, 60,  0 },
        { 90,  0,   0 },
        { 230, 0,   0 },
        { 32,  11,  119 },
        { 0,   74,  111 },
        { 81,  0,   81 }
    };
    if (rectangles.size() % 4 != 0 || rectangles.size() / 4 != classes.size()) {
        return;
    }

    for (size_t i = 0; i < classes.size(); i++) {
        int x = rectangles.at(i * 4);
        int y = rectangles.at(i * 4 + 1);
        int w = rectangles.at(i * 4 + 2);
        int h = rectangles.at(i * 4 + 3);

        int cls = classes.at(i) % colors.size();  // color of a bounding box line

        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (w < 0) w = 0;
        if (h < 0) h = 0;

        if (static_cast<std::size_t>(x) >= width) { x = width - 1; w = 0; thickness = 1; }
        if (static_cast<std::size_t>(y) >= height) { y = height - 1; h = 0; thickness = 1; }

        if (static_cast<std::size_t>(x + w) >= width) { w = width - x - 1; }
        if (static_cast<std::size_t>(y + h) >= height) { h = height - y - 1; }

        thickness = std::min(std::min(thickness, w / 2 + 1), h / 2 + 1);

        size_t shift_first;
        size_t shift_second;
        for (int t = 0; t < thickness; t++) {
            shift_first = (y + t) * width * 3;
            shift_second = (y + h - t) * width * 3;
            for (int ii = x; ii < x + w + 1; ii++) {
                data[shift_first + ii * 3] = colors.at(cls).red();
                data[shift_first + ii * 3 + 1] = colors.at(cls).green();
                data[shift_first + ii * 3 + 2] = colors.at(cls).blue();
                data[shift_second + ii * 3] = colors.at(cls).red();
                data[shift_second + ii * 3 + 1] = colors.at(cls).green();
                data[shift_second + ii * 3 + 2] = colors.at(cls).blue();
            }
        }

        for (int t = 0; t < thickness; t++) {
            shift_first = (x + t) * 3;
            shift_second = (x + w - t) * 3;
            for (int ii = y; ii < y + h + 1; ii++) {
                data[shift_first + ii * width * 3] = colors.at(cls).red();
                data[shift_first + ii * width * 3 + 1] = colors.at(cls).green();
                data[shift_first + ii * width * 3 + 2] = colors.at(cls).blue();
                data[shift_second + ii * width * 3] = colors.at(cls).red();
                data[shift_second + ii * width * 3 + 1] = colors.at(cls).green();
                data[shift_second + ii * width * 3 + 2] = colors.at(cls).blue();
            }
        }
    }
}



/**
 * Write output data to image
 * \param name - image name
 * \param data - output data
 * \param classesNum - the number of classes
 * \return false if error else true
 */

static UNUSED bool writeOutputBmp(unsigned char *data, size_t height, size_t width, std::ostream &outFile) {
    unsigned char file[14] = {
            'B', 'M',           // magic
            0, 0, 0, 0,         // size in bytes
            0, 0,               // app data
            0, 0,               // app data
            40+14, 0, 0, 0      // start of data offset
    };
    unsigned char info[40] = {
            40, 0, 0, 0,        // info hd size
            0, 0, 0, 0,         // width
            0, 0, 0, 0,         // height
            1, 0,               // number color planes
            24, 0,              // bits per pixel
            0, 0, 0, 0,         // compression is none
            0, 0, 0, 0,         // image bits size
            0x13, 0x0B, 0, 0,   // horz resolution in pixel / m
            0x13, 0x0B, 0, 0,   // vert resolution (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
            0, 0, 0, 0,         // #colors in palette
            0, 0, 0, 0,         // #important colors
    };

    if (height > (size_t)std::numeric_limits<int32_t>::max || width > (size_t)std::numeric_limits<int32_t>::max) {
        THROW_IE_EXCEPTION << "File size is too big: " << height << " X " << width;
    }

    int padSize  = static_cast<int>(4 - (width * 3) % 4) % 4;
    int sizeData = static_cast<int>(width * height * 3 + height * padSize);
    int sizeAll  = sizeData + sizeof(file) + sizeof(info);

    file[ 2] = (unsigned char)(sizeAll      );
    file[ 3] = (unsigned char)(sizeAll >>  8);
    file[ 4] = (unsigned char)(sizeAll >> 16);
    file[ 5] = (unsigned char)(sizeAll >> 24);

    info[ 4] = (unsigned char)(width      );
    info[ 5] = (unsigned char)(width >>  8);
    info[ 6] = (unsigned char)(width >> 16);
    info[ 7] = (unsigned char)(width >> 24);

    int32_t negativeHeight = -(int32_t)height;
    info[ 8] = (unsigned char)(negativeHeight      );
    info[ 9] = (unsigned char)(negativeHeight >>  8);
    info[10] = (unsigned char)(negativeHeight >> 16);
    info[11] = (unsigned char)(negativeHeight >> 24);

    info[20] = (unsigned char)(sizeData      );
    info[21] = (unsigned char)(sizeData >>  8);
    info[22] = (unsigned char)(sizeData >> 16);
    info[23] = (unsigned char)(sizeData >> 24);

    outFile.write(reinterpret_cast<char*>(file), sizeof(file));
    outFile.write(reinterpret_cast<char*>(info), sizeof(info));

    unsigned char pad[3] = {0, 0, 0};

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            unsigned char pixel[3];
            pixel[0] = data[y*width*3 + x*3];
            pixel[1] = data[y*width*3 + x*3 + 1];
            pixel[2] = data[y*width*3 + x*3 + 2];
            outFile.write(reinterpret_cast<char *>(pixel), 3);
        }
        outFile.write(reinterpret_cast<char *>(pad), padSize);
    }

    return true;
}

static std::vector<std::pair<std::string, InferenceEngine::InferenceEngineProfileInfo>>
perfCountersSorted(std::map<std::string, InferenceEngine::InferenceEngineProfileInfo> perfMap) {
    using perfItem = std::pair<std::string, InferenceEngine::InferenceEngineProfileInfo>;
    std::vector<perfItem> sorted;
    for (auto &kvp : perfMap) sorted.push_back(kvp);

    std::stable_sort(sorted.begin(), sorted.end(),
                     [](const perfItem& l, const perfItem& r) {
                         return l.second.execution_index < r.second.execution_index;
                     });

    return sorted;
}

static UNUSED void printPerformanceCounts(const std::map<std::string, InferenceEngine::InferenceEngineProfileInfo>& performanceMap,
                                          std::ostream &stream, std::string deviceName,
                                          bool bshowHeader = true) {
    long long totalTime = 0;
    // Print performance counts
    if (bshowHeader) {
        stream << std::endl << "performance counts:" << std::endl << std::endl;
    }

    auto performanceMapSorted = perfCountersSorted(performanceMap);

    for (const auto & it : performanceMapSorted) {
        std::string toPrint(it.first);
        const int maxLayerName = 30;

        if (it.first.length() >= maxLayerName) {
            toPrint  = it.first.substr(0, maxLayerName - 4);
            toPrint += "...";
        }


        stream << std::setw(maxLayerName) << std::left << toPrint;
        switch (it.second.status) {
        case InferenceEngine::InferenceEngineProfileInfo::EXECUTED:
            stream << std::setw(15) << std::left << "EXECUTED";
            break;
        case InferenceEngine::InferenceEngineProfileInfo::NOT_RUN:
            stream << std::setw(15) << std::left << "NOT_RUN";
            break;
        case InferenceEngine::InferenceEngineProfileInfo::OPTIMIZED_OUT:
            stream << std::setw(15) << std::left << "OPTIMIZED_OUT";
            break;
        }
        stream << std::setw(30) << std::left << "layerType: " + std::string(it.second.layer_type) + " ";
        stream << std::setw(20) << std::left << "realTime: " + std::to_string(it.second.realTime_uSec);
        stream << std::setw(20) << std::left << "cpu: "  + std::to_string(it.second.cpu_uSec);
        stream << " execType: " << it.second.exec_type << std::endl;
        if (it.second.realTime_uSec > 0) {
            totalTime += it.second.realTime_uSec;
        }
    }
    stream << std::setw(20) << std::left << "Total time: " + std::to_string(totalTime) << " microseconds" << std::endl;
    std::cout << std::endl;
    std::cout << "Full device name: " << deviceName << std::endl;
    std::cout << std::endl;
}

static UNUSED void printPerformanceCounts(InferenceEngine::InferRequest request, std::ostream &stream, std::string deviceName, bool bshowHeader = true) {
    auto performanceMap = request.GetPerformanceCounts();
    printPerformanceCounts(performanceMap, stream, deviceName, bshowHeader);
}

inline std::map<std::string, std::string> getMapFullDevicesNames(InferenceEngine::Core& ie, std::vector<std::string> devices) {
    std::map<std::string, std::string> devicesMap;
    InferenceEngine::Parameter p;
    for (std::string& deviceName : devices) {
        if (deviceName != "") {
            try {
                p = ie.GetMetric(deviceName, METRIC_KEY(FULL_DEVICE_NAME));
                devicesMap.insert(std::pair<std::string, std::string>(deviceName, p.as<std::string>()));
            }
            catch (InferenceEngine::details::InferenceEngineException &) {
            }
        }
    }
    return devicesMap;
}

inline std::string getFullDeviceName(std::map<std::string, std::string>& devicesMap, std::string device) {
    std::map<std::string, std::string>::iterator it = devicesMap.find(device);
    if (it != devicesMap.end()) {
        return it->second;
    } else {
        return "";
    }
}

inline std::string getFullDeviceName(InferenceEngine::Core& ie, std::string device) {
    InferenceEngine::Parameter p;
    try {
        p = ie.GetMetric(device, METRIC_KEY(FULL_DEVICE_NAME));
        return  p.as<std::string>();
    }
    catch (InferenceEngine::details::InferenceEngineException &) {
        return "";
    }
}

/**
 * @brief This class represents an object that is found by an object detection net
 */
class DetectedObject {
public:
    int objectType;
    float xmin, xmax, ymin, ymax, prob;
    bool difficult;

    DetectedObject(int _objectType, float _xmin, float _ymin, float _xmax, float _ymax, float _prob, bool _difficult = false)
        : objectType(_objectType), xmin(_xmin), xmax(_xmax), ymin(_ymin), ymax(_ymax), prob(_prob), difficult(_difficult) {
    }

    DetectedObject(const DetectedObject& other) = default;

    static float ioU(const DetectedObject& detectedObject1_, const DetectedObject& detectedObject2_) {
        // Add small space to eliminate empty squares
        float epsilon = 0;  // 1e-5f;

        DetectedObject detectedObject1(detectedObject1_.objectType,
                (detectedObject1_.xmin - epsilon),
                (detectedObject1_.ymin - epsilon),
                (detectedObject1_.xmax- epsilon),
                (detectedObject1_.ymax- epsilon), detectedObject1_.prob);
        DetectedObject detectedObject2(detectedObject2_.objectType,
                (detectedObject2_.xmin + epsilon),
                (detectedObject2_.ymin + epsilon),
                (detectedObject2_.xmax),
                (detectedObject2_.ymax), detectedObject2_.prob);

        if (detectedObject1.objectType != detectedObject2.objectType) {
            // objects are different, so the result is 0
            return 0.0f;
        }

        if (detectedObject1.xmax < detectedObject1.xmin) return 0.0;
        if (detectedObject1.ymax < detectedObject1.ymin) return 0.0;
        if (detectedObject2.xmax < detectedObject2.xmin) return 0.0;
        if (detectedObject2.ymax < detectedObject2.ymin) return 0.0;


        float xmin = (std::max)(detectedObject1.xmin, detectedObject2.xmin);
        float ymin = (std::max)(detectedObject1.ymin, detectedObject2.ymin);
        float xmax = (std::min)(detectedObject1.xmax, detectedObject2.xmax);
        float ymax = (std::min)(detectedObject1.ymax, detectedObject2.ymax);

        // Caffe adds 1 to every length if the box isn't normalized. So do we...
        float addendum;
        if (xmax > 1 || ymax > 1)
            addendum = 1;
        else
            addendum = 0;

        // intersection
        float intr;
        if ((xmax >= xmin) && (ymax >= ymin)) {
            intr = (a