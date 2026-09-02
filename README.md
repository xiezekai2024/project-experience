# 项目经历

本仓库整理了简历中的两个机器人与嵌入式项目，分别保存核心源码、硬件验证材料和运行说明。

## 目录

### 1. 无人机车载降落锁紧系统

基于 ROS2 构建跨设备 GPIO 控制原型：控制端发布 `gpio_control` 指令，Jetson Orin NX 订阅后通过 `libgpiod` 驱动 GPIO，完成继电器/电磁锁验证链路。目录中还包含 Jetson pinmux/设备树排查记录和 STM32F103 早期 GPIO 验证工程。

入口文件：

- `01_无人机车载降落锁紧系统/01_ROS2_GPIO控制/key_publisher.py`
- `01_无人机车载降落锁紧系统/01_ROS2_GPIO控制/gpio_subscriber.py`

### 2. YOLOv11 与 RGB-D 视觉引导的农业机器人采摘系统

在 Linux/C++17 环境中集成 YOLOv11 ONNX Runtime、Azure Kinect RGB-D、OpenCV、PCL、手眼坐标变换和 TCP 通信，实现目标检测、深度定位、坐标转换及机械臂位姿发送原型。

入口文件：

- `02_YOLOv11_RGBD农业机器人采摘/01_主源码_Linux完整版/main.cpp`
- `02_YOLOv11_RGBD农业机器人采摘/01_主源码_Linux完整版/CMakeLists.txt`

## 整理说明

- 未收录未公开专利、内部汇报、团队报名材料和个人信息。
- 未收录厂商手册、第三方论文、SDK、数据集、构建目录、重复源码副本和带本机路径的早期实验代码。
- 源码中的开发机绝对路径已改为仓库相对路径或 CMake 参数，机械臂 IP/端口改为运行参数。
- 农业机器人项目中的 YOLO 推理模块基于 GPL-3.0 上游项目修改，相关许可和上游说明保留在项目目录中。

