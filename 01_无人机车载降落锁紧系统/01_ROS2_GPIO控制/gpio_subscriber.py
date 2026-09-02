import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import gpiod

# GPIO 配置
CHIP_NAME = "gpiochip0"
LINE_OFFSET = 105  # GPIO01
CONSUMER = "orin_gpio_ros2"

class GpioSubscriber(Node):
    def __init__(self):
        super().__init__('gpio_subscriber')
        
        # 初始化 GPIO
        self.chip = gpiod.Chip(CHIP_NAME)
        self.line = self.chip.get_line(LINE_OFFSET)
        # 请求输出模式，初始为低电平
        self.line.request(consumer=CONSUMER, type=gpiod.LINE_REQ_DIR_OUT, default_vals=[0])
        self.current_state = 0
        
        # 创建订阅者
        self.subscription = self.create_subscription(
            String,
            'gpio_control',
            self.listener_callback,
            10)
        self.get_logger().info(f"GPIO 订阅者已就绪。正在监听 GPIO{LINE_OFFSET}...")

    def listener_callback(self, msg):
        if msg.data == 'toggle':
            # 翻转状态：0 变 1, 1 变 0
            self.current_state = 1 - self.current_state
            self.line.set_value(self.current_state)
            
            state_str = "HIGH" if self.current_state == 1 else "LOW"
            self.get_logger().info(f"收到指令！电平已切换至: {state_str}")

    def __del__(self):
        # 析构函数：确保节点关闭时释放 GPIO
        if hasattr(self, 'line'):
            self.line.release()
        if hasattr(self, 'chip'):
            self.chip.close()

def main(args=None):
    rclpy.init(args=args)
    gpio_sub = GpioSubscriber()

    try:
        rclpy.spin(gpio_sub)
    except KeyboardInterrupt:
        pass
    finally:
        gpio_sub.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()