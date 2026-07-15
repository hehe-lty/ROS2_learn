import rclpy
from rclpy.node import Node

from demo_python_pkg.person_node import PersonNode

class WriterNode(PersonNode):
    def __init__(self, node_name:str, name: str, age: str, book: str) -> None:
        print("WriterNode __init__方法被调用了,添加了一个属性")
        super().__init__(node_name, name, age) #调用父类的构造方法
        self.get_logger().info("WriterNode 初始化完成，新增 book 属性")
        self.book = book

    def look(self):
        print("正在看《%s》"%self.book)



def main():
    rclpy.init()
    node = WriterNode("xiaoyu","小鱼","18","ROS2学习")
    node.eat("西红柿鸡蛋面")
    node.look()

    rclpy.spin(node)
    rclpy.shutdown()
   