# 基于 ONNX Runtime 的 YOLOv11 C++ 推理引擎

## 项目概述

本项目使用 C++ 实现基于 ONNX Runtime 的 YOLOv11 推理引擎。项目主要参考了 [FourierMourier 的 yolov8-onnx-cpp](https://github.com/FourierMourier/yolov8-onnx-cpp)，并在原项目 [K4HVH 的 YOLOv8-ONNXRuntime-CPP](https://github.com/K4HVH/YOLOv8-ONNXRuntime-CPP) 基础上升级而来。

本实现旨在提供精简、高效且便于按实际需求修改的目标检测流程。该版本针对现代计算机进行了性能优化，并使用 AVX 指令提升推理速度。

## 主要特点

- **高性能：** 针对速度进行优化，支持在循环中连续高速推理。
- **结构简洁：** 精简代码结构，专注于目标检测功能。
- **易于扩展：** 便于根据不同使用场景进行修改和扩展。
- **精度更高：** 在相近推理速度下，YOLOv11 相比 YOLOv8 具有更高的检测精度。
- **推理更快：** 根据上游项目测试，在模型规模相同的情况下，本项目的推理速度约为 YOLOv8 版本的 4 倍。

## 环境要求

- **ONNX Runtime：** 请确保系统中已安装 ONNX Runtime。
- **OpenCV：** 用于图像处理和结果显示。
- **C++ 编译器：** 需支持 C++11 或更高版本。

## 快速开始

### 安装

1. 克隆原始上游仓库：

```sh
git clone https://github.com/K4HVH/YOLOv11-ONNXRuntime-CPP
cd YOLOv11-ONNXRuntime-CPP
```

2. 安装依赖：

请确保系统中已经安装 ONNX Runtime 和 OpenCV。ONNX Runtime 的安装说明可参阅[官方文档](https://onnxruntime.ai/)。

### 编译

1. 配置项目：

编辑项目根目录中的 `CMakeLists.txt`，将 `"path/to/onnxruntime"` 替换为本机 ONNX Runtime 的实际安装路径。

```cmake
# ONNX Runtime 的安装路径
set(ONNXRUNTIME_DIR "path/to/onnxruntime")
```

2. 构建项目：

```sh
mkdir build
cd build
cmake ..
make
```

### 运行推理

1. 运行可执行程序：

```sh
./yolo_inference
```

2. 使用图片测试：

修改 `main.cpp` 中的 `imagePath` 变量，使其指向需要测试的图片。

## 项目结构

- **`main.cpp`：** 程序入口，负责初始化推理器并对示例图片执行检测。
- **`engine.hpp`：** YOLOv11 推理器类的头文件，定义数据结构和方法。
- **`engine.cpp`：** YOLOv11 推理器的实现，包含预处理、前向推理和后处理流程。

## 使用示例

以下代码展示了如何在 `main.cpp` 中调用推理器：

```cpp
#include "engine.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

int main()
{
    std::wstring modelPath = L"yolo11n.onnx";
    const char* logid = "yolo_inference";
    const char* provider = "CPU"; // 也可以使用 CUDA

    YoloInferencer inferencer(modelPath, logid, provider);

    std::string imagePath = "test.jpg"; // 替换为测试图片路径
    cv::Mat image = cv::imread(imagePath);

    if (image.empty()) {
        std::cerr << "错误：无法加载图片！" << std::endl;
        return -1;
    }

    std::vector<Detection> detections = inferencer.infer(image, 0.4, 0.5);

    for (const auto& detection : detections) {
        cv::rectangle(image, detection.box, cv::Scalar(0, 255, 0), 2);
        std::cout << "检测结果：类别=" << detection.class_id << ", 置信度=" << detection.confidence
            << ", x=" << detection.box.x << ", y=" << detection.box.y
            << ", 宽度=" << detection.box.width << ", 高度=" << detection.box.height << std::endl;
    }

    cv::imshow("检测结果", image);
    cv::waitKey(0);

    return 0;
}
```

## 参与贡献

欢迎参与项目改进。如需提交修改，可以创建议题或提交拉取请求。

## 开源许可证

本项目采用 GPL-3.0 许可证，完整条款见同目录下的 `LICENSE` 文件。

## 致谢

本项目主要参考了 [yolov8-onnx-cpp](https://github.com/FourierMourier/yolov8-onnx-cpp)，并由原项目 [YOLOv8-ONNXRuntime-CPP](https://github.com/K4HVH/YOLOv8-ONNXRuntime-CPP) 更新而来。
