# This repo is for an SO issue

### How to run

```bash
clang++ main.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core mcjit native` -O3 -g -o main
```

### Link to issue
https://stackoverflow.com/questions/50864799/gep-segmentation-fault-llvm-c-api
