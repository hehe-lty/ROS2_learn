#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/static_transform_broadcaster.h" //静态坐标变换广播器
#include "geometry_msgs/msg/transform_stamped.hpp" //坐标变换消息
#include "tf2/LinearMath/Quaternion.h" //提供tf2::Quaternion类
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp" //消息类型转换函数

class StaticTFBroadcaster : public rclcpp::Node
{
private:
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> broadcaster_; //静态坐标变换广播器

public:
    StaticTFBroadcaster() : Node("static_tf_broadcaster")
    {
        // 创建静态坐标变换广播器
        broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);
        this->publish_tf(); //发布静态坐标变换
        RCLCPP_INFO(this->get_logger(), "发布map坐标系到target_point坐标系的静态坐标变换");
    }

    void publish_tf()
    {
        geometry_msgs::msg::TransformStamped transform;

        transform.header.stamp = this->get_clock()->now(); //设置时间戳

        // 设置父坐标系和子坐标系
        transform.header.frame_id = "map"; //父坐标系
        transform.child_frame_id = "target_point"; //子坐标系

        // 设置平移参数
        transform.transform.translation.x = 5.0; //x方向平移5
        transform.transform.translation.y = 3.0; //y方向平移3
        transform.transform.translation.z = 0.0; //z方向平移0

        // 设置旋转参数，使用欧拉角转四元数
        tf2::Quaternion q;
        q.setRPY(0.0, 0.0, 60*M_PI/180.0); //设置绕Z轴旋转60度
        transform.transform.rotation = tf2::toMsg(q);//将tf2::Quaternion类型转换为geometry_msgs::msg::Quaternion类型

        // 发布静态坐标变换
        broadcaster_->sendTransform(transform);
    }

};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<StaticTFBroadcaster>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}