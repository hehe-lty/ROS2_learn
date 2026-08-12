#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_listener.h" //坐标变换监听器
#include "geometry_msgs/msg/transform_stamped.hpp" //坐标变换消息
#include "tf2/LinearMath/Quaternion.h" //提供tf2::Quaternion类
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp" //消息类型转换函数
#include "chrono" //提供时间相关函数
#include "tf2/utils.h" //提供欧拉角
#include "tf2_ros/buffer.h" //提供tf2_ros::Buffer类

using namespace std::chrono_literals; //使用时间单位 s ms

class TFListener : public rclcpp::Node
{
private:
    std::shared_ptr<tf2_ros::TransformListener> listener_; //坐标变换监听器
    rclcpp::TimerBase::SharedPtr timer_; //定时器
    std::shared_ptr<tf2_ros::Buffer> buffer_; //坐标变换缓冲区

public:
    TFListener() : Node("tf_listener")
    {
        // 创建坐标变换监听器        
        this->buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        this->listener_ = std::make_shared<tf2_ros::TransformListener>(*buffer_,this);
        // this->listen_tf(); //监听坐标变换
        timer_ = this->create_wall_timer(1s, std::bind(&TFListener::get_Transform, this)); //创建定时器，每100ms调用一次listen_tf函数
        RCLCPP_INFO(this->get_logger(), "监听base_link坐标系到target_point坐标系的坐标变换");

    }

    void get_Transform()
    {
        // 到buffer_中查询坐标变换
        try
        {
            //查询坐标关系
            const auto transform = buffer_->lookupTransform("base_link", "target_point", this->get_clock()->now(), rclcpp::Duration::from_seconds(1.0));
            // 获取查询结果
            auto translation = transform.transform.translation;
            auto rotation = transform.transform.rotation;
            double y,p,r;
            tf2::getEulerYPR(rotation, y, p, r);
            RCLCPP_INFO(this->get_logger(), "坐标变换查询成功: 平移参数: x=%.2f, y=%.2f, z=%.2f, 旋转参数: roll=%.2f, pitch=%.2f, yaw=%.2f", 
                translation.x, translation.y, translation.z, r, p, y);    
            
            // RCLCPP_INFO(this->get_logger(), "坐标变换查询成功");
            // RCLCPP_INFO(this->get_logger(), "父坐标系: %s, 子坐标系: %s", transform.header.frame_id.c_str(), transform.child_frame_id.c_str());
            // RCLCPP_INFO(this->get_logger(), "平移参数: x=%.2f, y=%.2f, z=%.2f", transform.transform.translation.x, transform.transform.translation.y, transform.transform.translation.z);
            // RCLCPP_INFO(this->get_logger(), "旋转参数: x=%.2f, y=%.2f, z=%.2f, w=%.2f", transform.transform.rotation.x, transform.transform.rotation.y, transform.transform.rotation.z, transform.transform.rotation.w);
        }
        catch (const std::exception & e)
        {
            RCLCPP_WARN(this->get_logger(), "坐标变换查询失败: %s", e.what());
        }
    }

};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared< TFListener>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}