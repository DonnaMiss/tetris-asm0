[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]

# Deep Learning Inference Framework Speed Benchmarks - CPU

## About 
This project benchmarks the performance of various popular deep learning inference frameworks on x86_64 CPU.
The intention is to determine which has the lowest latency, and how the number of threads used for inference impacts performance.
Thead dependency can be used to optimize CPU throughput (ex. run multiple instances in parallel each only using a reduced number of threads).

## Test Specs
The benchmarks are performed on `Dual Intel(R) Xeon(R) CPU E5-2630 v4 @ 2.20GHz`, 20 cores total, 128 Gb of ram.
The model used for inference is the [Arcface](https://docs.openvinotoolkit.org/latest/omz_models_pub