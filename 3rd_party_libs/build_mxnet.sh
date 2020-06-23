# Download ncnn
test -e incubator-mxnet || git clone --recursive https://github.com/apache/incubator-mxnet.git

cd incubator-mxnet
git reset --hard 2fc0706 # 