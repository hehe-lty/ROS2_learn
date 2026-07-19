#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"
#include <chrono> 
#include "chapt4_interfaces/srv/patrol.hpp"

using Patrol = chapt4_interfaces::srv::Patrol; //定义服务类型


class TurtleControlNode: public rclcpp::Node
{
private:
    rclcpp::Service<Patrol>::SharedPtr patrol_service_; //服务智能指针
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_; //发布者智能指针
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscriber_; //订阅者智能指针
    double target_x_{3.0};
    double target_y_{6.0};
    double k_{1.0}; //比例系数·
    double max_speed_{3.0}; //最大速度


public:
    explicit TurtleControlNode(const std::string& node_name): Node(node_name)
    { 
        patrol_service_ = this->create_service<Patrol>(
            "patrol", [&](const Patrol::Request::SharedPtr request, 
            Patrol::Response::SharedPtr response) -> void {
                if((0 < request -> target_x&&request -> target_x < 12.0f)
                &&(0 < request -> target_y&&request -> target_y < 12.0f))
                {
                    this->target_x_ = request->target_x;
                    this->target_y_ = request->target_y;
                    response->result = Patrol::Response::SUCCESS;
                }else{
                    response->result = Patrol::Response::FALL;
                }
            });
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        subscriber_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 10, std::bind(&
        TurtleControlNode::on_pose_received_, this, std::placeholders::_1));
        // timer_ = this->create_wall_timer(1000ms, std::bind(&TurtleControlNode::timer_callback, this));
    }

    void on_pose_received_(const turtlesim::msg::Pose::SharedPtr pose)//参数：收到数据的共享指针
    {
        //1.获取当前位置
        auto current_x = pose->x;
        auto current_y = pose->y;
        RCLCPP_INFO(get_logger(),"当前x=%f,y=%f", current_x, current_y);
        //2.计算当前海龟和目标之间的距离差和角度差
        auto distance = std::sqrt(
            (target_x_ - current_x)*(target_x_ - current_x) +
            (target_y_ - current_y)*(target_y_ - current_y)
        );
        auto angle = std::atan2(target_y_ - current_y, target_x_ - current_x) - pose->theta;
        //3.根据距离差和角度差，设置海龟的移动速度和方向
        auto msg = geometry_msgs::msg::Twist();
        if(distance > 0.1)
        { 
            if (fabs(angle) > 0.2){
                msg.angular.z = fabs(angle);
            }else{
                msg.linear.x = k_*distance;
            }
        }
        //4.限制线速度最大值
        if(msg.linear.x > max_speed_){
            msg.linear.x = max_speed_;
        }
        //5.发布消息
        publisher_->publish(msg);  
    }
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<TurtleControlNode>("turtle_control");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}


