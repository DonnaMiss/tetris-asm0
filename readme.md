[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]

# Deep Learning Inference Framework Speed Benchmarks - CPU

## About 
This project benchmarks the performance of various popular deep learning inference frameworks on x86_64 CPU.
The intention is to determine which has the lowest latency, and how the number of threads used for inference impacts performance.
Thead dependency can be used to optimize CPU throughput (ex. run multiple instances in parallel each only using a reduced number of threads).

## Test Specs
The benchmarks are performed on `Dual Intel(R) Xeon(R) CPU E5-2630 v4 @ 2.20GHz`, 20 cores total, 128 Gb of ram.
The model used for inference is the [Arcface](https://docs.openvinotoolkit.org/latest/omz_models_public_face_recognition_resnet100_arcface_face_recognition_resnet100_arcface.html) ResNet100 with 65.131 MParams and 24.209 GFLOPs. 
The model was trained in mxnet.
The model input is a 112x112 rgb image. 
The time taken to read the image into memory and decode the image to rgb format is *not* included in the inference time.
The time taken to convert the rgb image buffer into the framework-specific expected network input format *is* included in the inference time. 
First inference time is discarded to ensure all network weights and params have been loaded. Memory usage refers to maximum resident set size.
The number of inferences used to generate the average time is on the order of 1000. 
*Unrestricted threads* in tables below means allowing the inference framework to choose how many threads it wants to use on it's own.
Runtime dependencies refers to dependencies which must be linked and are *not* standard system libraries such as `libstdc++.so`. 

## Usage
- Start by navigating to the `3rd_party_libs` directory and compile the inference frameworks by running the provided shell scripts.
- `./deeplearningInferenceBenchmarks /path/to/model/directory number_of_threads (optional)`

## Summary

## Results

![alt text](./images/speed_line.png)
![alt text](./images/speed_bar.png)
![alt text](./images/memory_bar.png)


### mxnet
- Github [link](https://github.com/apache/incubator-mxnet)
- git release tag: `1.8.0.rc3`
- Using MKL backend, download instructions [here](https://software.intel.com/content/www/us/en/develop/articles/installing-intel-oneapi-toolkits-via-apt.html)
- CMake build arguments: `-DUSE_CPP_PACKAGE=1 -DBUILD_CPP_EXAMPLES=OFF -DUSE_CUDA=0 -DUSE_MKL_IF_AVAILABLE=1 -DUSE_BLAS=mkl -DUSE_OPENCV=0 -DUSE_LAPACK=0 -DUSE_OPENMP=1 -DMKL_INCLUDE_DIR=/opt/intel/oneapi/mkl/latest/include -DMKL_RT_LIBRARY=/opt/intel/oneapi/mkl/latest/lib/intel64/libmkl_rt.so -DCMAKE_BUILD_TYPE=Release`
- Runtime dependencies: `libmkl_rt.so libomp.so libpthread.so`
- Maintains output integrity: yes

| Number of threads     | Average inference speed   | Memory usage  |
|-------------------    |-------------------------  |-------------- |
| 1                     | 352 ms                    | 0.52 Gb       |
| 2                     | 188 ms                    | 0.52 Gb       |
| 3                     | 134 ms                    | 0.52 Gb       |
| 4                     | 108 ms                    | 0.53 Gb       |
| 5                     | 93 ms                     | 0.52 Gb       |
| 6                     | 82 ms               