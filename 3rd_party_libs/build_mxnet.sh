# Download ncnn
test -e incubator-mxnet || git clone --recursive https://github.com/apache/incubator-mxnet.git

cd incubator-mxnet
git reset --hard 2fc0706 # 1.8.0.rc3
git submodule update --init --recursive

mkdir build_amd64
cd build_amd64

