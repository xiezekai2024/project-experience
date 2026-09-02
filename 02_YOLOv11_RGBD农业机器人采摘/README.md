# YOLOv11 与 RGB-D 视觉引导的农业机器人采摘系统

## 项目概述

项目将目标检测、RGB-D 定位、手眼坐标变换和机械臂通信串成一条实验链路：

```text
Azure Kinect RGB-D
  -> YOLOv11 ONNX Runtime 检测
  -> HSV 分割与目标质心
  -> 深度对齐和三维点转换
  -> 手眼矩阵变换
  -> TCP 发送机械臂位姿
  -> 等待 Finish 应答
```

## 核心目录

- `01_主源码_Linux完整版/`：Linux/C++17 主版本，包含相机、推理、定位、坐标变换和 TCP 通信源码。

主程序模块：

- `main.cpp`：完整流程编排。
- `engine.cpp` / `include/engine.hpp`：ONNX Runtime 推理和 NMS。
- `algo.cpp` / `include/algo.h`：HSV 细化、质心和手眼变换。
- `kinect_camera.cpp` / `include/kinect_camera.h`：RGB-D 采集、深度对齐与点云。
- `tcp_comm.cpp` / `include/tcp_comm.h`：机械臂 TCP 通信。

## 依赖

- Linux、CMake 3.10+、C++17
- ONNX Runtime GPU 1.17.0
- Azure Kinect Sensor SDK
- OpenCV、Eigen3、PCL
- TIFF、GeoTIFF、GDAL、FFI

## 构建

```bash
cd 01_主源码_Linux完整版
cmake -S . -B build \
  -Dk4a_DIR=/path/to/Azure-Kinect-Sensor-SDK \
  -DONNXRUNTIME_DIR=/path/to/onnxruntime-linux-x64-gpu-1.17.0
cmake --build build -j
```

## 运行

运行参数依次为机械臂服务器 IP、端口和模型路径：

```bash
./build/yolo_inference 192.168.10.100 8088 ./a3.onnx
```

不传参数时默认连接 `127.0.0.1:8088`，并读取当前目录的 `a3.onnx`。

## 当前边界

- 手眼矩阵、阈值等实验参数仍在源码中，后续应迁移到配置文件。
- TCP 协议仍需补充消息边界、总超时、重连和完整发送处理。
- 当前稳定支持三维位置估计；点云姿态估计和聚类仍属于实验代码。

## 上游许可

YOLO 推理模块基于 `K4HVH/YOLOv11-ONNXRuntime-CPP` 及其注明的上游项目修改，继续遵循 GPL-3.0。完整许可见 `01_主源码_Linux完整版/LICENSE`，原上游说明见同目录 `README.md`。

