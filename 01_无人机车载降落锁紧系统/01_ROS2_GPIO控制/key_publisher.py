import rclpy
from rclpy.node import Node
from std_msgs.msg import String
from pynput import keyboard

class KeyPublisher(Node):
    def __init__(self):
        super().__init__('key_publisher')
        self.publisher_ = self.create_publisher(String, 'gpio_control', 10)
        self.get_logger().info("键盘监听已启动。按下 'a' 键发布翻转信号，按 'Esc' 退出。")

    def on_press(self, key):
        try:
            # 检测是否按下了 'a'
            if key.char == 'a':
                msg = String()
                msg.data = 'toggle'
                self.publisher_.publish(msg)
                self.get_logger().info("已发送翻转指令 (a pressed)")
        except AttributeError:
            pass

    def on_release(self, key):
        if key == keyboard.Key.esc:
            # 停止监听
            return False

def main(args=None):
    rclpy.init(args=args)
    node = KeyPublisher()

    # 使用 pynput 监听键盘
    with keyboard.Listener(on_press=node.on_press, on_release=node.on_release) as listener:
        # 在单独的线程中运行 ROS 2 节点，以便监听器可以工作
        import threading
        thread = threading.Thread(target=rclpy.spin, args=(node,), daemon=True)
        thread.start()
        
        listener.join() # 等待监听器结束

    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()