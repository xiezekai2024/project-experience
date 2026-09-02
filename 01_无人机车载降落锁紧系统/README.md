# 无人机车载降落锁紧系统

## 项目概述

本项目验证了从 ROS2 话题到 Jetson GPIO，再到继电器/电磁锁执行端的控制链路。

```text
键盘控制端
  -> key_publisher.py
  -> ROS2 topic: gpio_control
  -> gpio_subscriber.py
  -> libgpiod / gpiochip0 / line 105
  -> Jetson GPIO01
  -> 继电器或电磁锁
```

## 目录说明

- `01_ROS2_GPIO控制/`：ROS2 发布端和 Jetson GPIO 订阅端。
- `02_Jetson设备树与引脚调试/`：GPIO01、PQ.05、line 105 的 pinmux 与设备树验证材料。
- `03_STM32早期GPIO验证/`：PA1、PC13 开漏输出的 CubeMX/Core 验证工程。

## 运行依赖

- ROS2、`rclpy`、`std_msgs`
- 控制端：`pynput`
- Jetson 端：Python libgpiod v1 API
- 已正确应用的 Jetson pinmux/DTB
- 两端一致的 ROS2 DDS 网络与 `ROS_DOMAIN_ID`

## 原型运行

Jetson 端：

```bash
python3 01_ROS2_GPIO控制/gpio_subscriber.py
```

控制端：

```bash
python3 01_ROS2_GPIO控制/key_publisher.py
```

按 `a` 发布一次 `toggle` 指令，按 `Esc` 退出控制端。

## 当前边界

当前代码是开环控制原型，尚未包含锁紧到位传感器、状态确认、失联保护、参数化配置和自动化测试。真实设备运行前应增加明确的 `LOCK/UNLOCK` 协议、超时回安全状态和硬件保护策略。

