import rclpy
from rclpy.node import Node
from chapt4_interfaces.srv import FaceDetector
import cv2
from ament_index_python import get_package_share_directory
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import os
from rcl_interfaces.srv import SetParameters
from rcl_interfaces.msg import Parameter, ParameterValue, ParameterType

class FaceDetectorClientNode(Node):
    def __init__(self):
        super().__init__('face_detector_client')
        self.bridge = CvBridge()
        self.defaut_image_path = os.path.join(get_package_share_directory(
        'demo_python_service'), 'resource/test3.jpg')
        self.get_logger().info("人脸检测客户端启动成功")
        self.client_ = self.create_client(FaceDetector, 'face_detect')
        self.image = cv2.imread(self.defaut_image_path)

    def call_set_parameters(self, parameters):
        """
        调用服务，修改参数
        """
        #1.创建客户端，等待服务上线
        update_param_client = self.create_client(SetParameters, '/face_detect_node/set_parameters')
        while update_param_client.wait_for_service(timeout_sec=1.0) is False:
            self.get_logger().warn("等待参数更新服务端上线！")
        #2.构造Request
        request = SetParameters.Request()
        request.parameters = parameters
        #3.调用服务端更新参数
        future = update_param_client.call_async(request)
        rclpy.spin_until_future_complete(self, future)#等待服务端处理完成
        response = future.result()
        return response
    
    def update_detect_model(self, model = 'hog'):
        """
        根据传入的modle构造Parameter，然后调用call_set_parameters更新服务参数
        """
        #1.创建参数对象
        param = Parameter()
        param.name = 'model'
        #2.创建参数值对象并赋值
        param_value = ParameterValue()
        param_value.string_value = model
        param_value.type = ParameterType.PARAMETER_STRING
        param.value = param_value
        #3.请求更新参数
        response = self.call_set_parameters([param])
        for result in response.results:
            self.get_logger().info(f"设置参数结果: {result.successful} {result.reason}")

            # if result.successful:
            #     self.get_logger().info('success')
            # else:
            #     self.get_logger().info('fail')

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
    node = FaceDetectorClientNode()
    # node.update_detect_model('hog')
    # node.send_request()
    node.update_detect_model('hog')
    node.send_request()
    rclpy.spin(node)
    rclpy.shutdown()