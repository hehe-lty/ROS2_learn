import rclpy
from rclpy.node import Node
from tf2_ros import TransformListener,Buffer #坐标监听器
from geometry_msgs.msg import TransformStamped #消息接口
from tf_transformations import quaternion_from_euler, euler_from_quaternion #欧拉角与四元数互相转换
import math #角度转弧度函数

class TFListener(Node):
    def __init__(self):
        super().__init__('tf_listener') #初始化节点
        self.buffer_ = Buffer()
        self.listener_ = TransformListener(self.buffer_, self) #创建坐标监听器对象
        self.timer_ = self.create_timer(1.0, self.get_transform) #创建定时器，每秒调用一次监听函数


    def get_transform(self):
        """
        实时查询坐标变换 buffer_
        """
        try:
            result = self.buffer_.lookup_transform('base_link', 'bottle_link', 
                    rclpy.time.Time(seconds=0.0), rclpy.duration.Duration(seconds=1.0))#查询坐标变换
            transform = result.transform
            self.get_logger().info(f'平移:{transform.translation}') #打印坐标变换
            self.get_logger().info(f'旋转:{transform.rotation}')
            rotation_euler = euler_from_quaternion([
                transform.rotation.x, 
                transform.rotation.y, 
                transform.rotation.z, 
                transform.rotation.w]) #四元数转欧拉角
            self.get_logger().info(f'旋转RPY:{rotation_euler}')

        except Exception as e:
            self.get_logger().error(f'查询TF时出错:{e}')



def main():
    rclpy.init() #初始化rclpy
    node = TFListener() #创建节点对象
    rclpy.spin(node) #循环等待回调函数
    rclpy.shutdown() #关闭rclpy

