#include "rclcpp/rclcpp.hpp"
#include "chapt4_interfaces/srv/patrol.hpp"
#include <chrono>
#include <ctime>

using Patrol = chapt4_interfaces::srv::Patrol; //定义服务类型
using namespace std::chrono_literals; //可以使用秒s


class PatrolClient : public rclcpp::Node
{
public:
    explicit PatrolClient(): Node("patrol_client")
    { 
        srand(time(NULL)); //随机数种子
        patrol_client_ = this->create_client<Patrol>("patrol"); //创建客户端
        timer_ = this->create_wall_timer(10s, [&]()->void{
            //1.检测服务是否上线
            while (!this->patrol_client_->wait_for_service(1s))
            {
                if(rclcpp::ok())
                {
                    RCLCPP_ERROR(this->get_logger(), "等待服务上线中rclcpp挂了！！！");
                    return;
                }
                RCLCPP_INFO(this->get_logger(), "等待服务上线中......");
                
            }
            //2.构造请求对象
            auto request = std::make_shared<Patrol::Request>();
            request->target_x = rand()%15;
            request->target_y = rand()%15;
            RCLCPP_INFO(this->get_logger(), "发送目标点：(%f,%f)",request->target_x,
            request->target_y);
            //3.发送请求
            this->patrol_client_->async_send_request(request, [&]
            (rclcpp::Client<Patrol>::SharedFuture result_future)->void{
                auto response = result_future.get();
                if(response->result == Patrol::Response::SUCCESS)
                RCLCPP_INFO(this->get_logger(), "成功到达目标点");
                else
                RCLCPP_ERROR(this->get_logger(), "未成功到达目标点");
            });
            
        });

    }
private:
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Client<Patrol>::SharedPtr patrol_client_;

};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<PatrolClient>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}


