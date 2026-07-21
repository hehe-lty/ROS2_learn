import rclpy
from rclpy.node import Node
from chapt4_interfaces.srv import FaceDetector
import face_recognition
import cv2
from ament_index_python import get_package_share_directory
import os
from cv_bridge import CvBridge 
import time
from rcl_interfaces.msg import SetParametersResult

class FaceDetecNode(Node):
    def __init__(self):
        super().__init__('face_detect_node')
        self.service_ = self.create_service(FaceDetector, 'face_detect', self.detect_face_callback)
        self.bridge = CvBridge()
        ###4.4.1 参数服务
        self.declare_parameter("number_of_times_to_upsample", 1)  #添加参数
        self.declare_parameter("model", 'hog')  #添加参数
        self.number_of_times_to_upsample = self.get_parameter("number_of_times_to_upsample").value #使用参数
        self.model = self.get_parameter("model").value #使用参数

        self.defaut_image_path = os.path.join(get_package_share_directory('demo_python_service'), 'resource/default.jpg')
        self.get_logger().info("人脸检测服务启动成功")
        self.add_on_set_parameters_callback(self.paramenters_callback)
        
        #设置自身节点参数的方法
        # self.set_paramenters_callback([rclpy.Parameter("model", rclpy.Parameter.Type.STRING, "cnn")])
    ###4.4.2 在代码中订阅参数更新
    def paramenters_callback(self, paramters):
        '''
        当在参数更新时可以调用此函数，
        订阅参数更新，从而直观的看到参数变化。
        '''
        for paramter in paramters:
            self.get_logger().info(f"参数更新:{paramter.name}->{paramter.value}")
            if paramter.name == "number_of_times_to_upsample":
                self.number_of_times_to_upsample = paramter.value
            elif paramter.name == "model":
                self.model = paramter.value
        return SetParametersResult(successful=True)

    def detect_face_callback(self, request, response):
        if request.image.data:
             cv_image = self.bridge.imgmsg_to_cv2(request.image)
        else:
             cv_image = cv2.imread(self.defaut_image_path)
             self.get_logger().info(f"未传图像，使用默认图片:{self.defaut_image_path}")
        #cv_image为opencv格式的图像
        start_time = time.time()
        self.get_logger().info(f"开始识别人脸,耗时:{time.time()-start_time}")

        face_locations = face_recognition.face_locations(
        cv_image, number_of_times_to_upsample = self.number_of_times_to_upsample, model=self.model)

        response.use_time = time.time() - start_time
        response.number = len(face_locations)
        for top, right, bottom, left in face_locations:
            response.top.append(top)
            response.right.append(right)
            response.bottom.append(bottom)
            response.left.append(left)
        return response
   


def main():
    rclpy.init()
    node = FaceDetecNode()
    rclpy.spin(node)
    rclpy.shutdown()