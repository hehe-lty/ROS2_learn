import rclpy
from rclpy.node import Node

def main(args=None):
    rclpy.init(args=args) #初始化ROS 2
    node = Node('python_node') #创建节点
    node.get_logger().info('Hello ROS 2') #输出日志
    node.get_logger().warn('This is a Python node') #输出日志
    node.get_logger().error('This is an error message')
    rclpy.spin(node) #运行节点
    rclpy.shutdown() #关闭ROS 2
