import rclpy
from rclpy.node import Node
from chapt4_interfaces.srv import FaceDetector
import cv2
from ament_index_python import get_package_share_directory
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import os

class FaceDetectorClintNode(Node):
    def __init__(self):
        super().__init__('face_detector_clint')
        self.bridge = CvBridge()
        self.defaut_image_path = os.path.join(get_package_share_directory(
        'demo_python_service'), 'resource/test3.jpg')
        self.get_logger().info("人脸检测客户端启动成功")
        self.client_ = self.create_client(FaceDetector, 'face_detect')
        self.image = cv2.imread(self.defaut_image_path)

    def send_request(self):
        #1.判断服务端是否在线
        while self.client_.wait_for_service(timeout_sec=1.0) is False:
            self.get_logger().warn("等待服务端上线！")
        #2.构造Request
        request = FaceDetector.Request()
        request.image = self.bridge.cv2_to_imgmsg(self.image)
        #发送请求并等待处理完成
        future = self.client_.call_async(request)
        rclpy.spin_until_future_complete(self, future)#阻塞等待服务端处理完成
        #4.根据处理结果进行下一步操作
        response = future.result()
        self.get_logger().info(f"接收到的响应：人脸结果为：{response.number}, 耗时{response.use_time}")
        self.show_response(response)

    def show_response(self, response):
        for i in range(response.number):
            top = response.top[i]
            left = response.left[i]
            bottom = response.bottom[i]
            right = response.right[i]
            cv2.rectangle(self.image, (left, top), (right, bottom), (255, 0, 0), 4)
        cv2.imshow("Face Detecte Result", self.image)
        cv2.waitKey(0)



def main():
    rclpy.init()
    node = FaceDetectorClintNode()
    node.send_request()
    rclpy.spin(node)
    rclpy.shutdown()