#include "rclcpp/rclcpp.hpp"
#include "chapt4_interfaces/srv/patrol.hpp"
#include <chrono>
#include <ctime>
#include "rcl_interfaces/msg/parameter.hpp"
#include "rcl_interfaces/msg/parameter_value.hpp"
#include "rcl_interfaces/msg/parameter_type.hpp"
#include "rcl_interfaces/srv/set_parameters.hpp"

using SetP = rcl_interfaces::srv::SetParameters; //定义服务类型

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

    /*创建客户端发送请求，返回结果*/
    SetP::Response::SharedPtr call_set_parameter(const rcl_interfaces::msg::Parameter &param)
    {
        auto param_client = this->create_client<SetP>("/turtle_control/set_parameters"); //创建客户端
            //1.检测服务是否上线
            while (!param_client->wait_for_service(1s))
            {
                if(rclcpp::ok())
                {
                    RCLCPP_ERROR(this->get_logger(), "等待服务上线中rclcpp挂了！！！");
                    return nullptr;
                }
                RCLCPP_INFO(this->get_logger(), "等待服务上线中......");
                
            }
            //2.构造请求对象
            auto request = std::make_shared<SetP::Request>();
            request->parameters.push_back(param);

            //3.发送请求
            auto future = param_client->async_send_request(request);
            rclcpp::spin_until_future_complete(this->get_node_base_interface(), future);
            auto response = future.get();
            return response;

        
    }
    
    /*更新参数K函数*/
    void update_server_param_k(double k)
    {   
        //1.创建参数对象
        auto param = rcl_interfaces::msg::Parameter();
        param.name = "k";
        //2.设置参数类型和值
        auto param_value = rcl_interfaces::msg::ParameterValue();
        param_value.type = rcl_interfaces::msg::ParameterType::PARAMETER_DOUBLE;
        param_value.double_value = k;
        param.value = param_value;
        //3.调用服务更新参数并处理
        auto response = call_set_parameter(param);
        if(response==NULL)
        {
            RCLCPP_INFO(this->get_logger(), "更新参数失败，服务未响应");
            return;
        }
        for(auto result:response->results)
        {
            if(result.successful==false)
            RCLCPP_INFO(this->get_logger(), "更新参数失败,原因:%s",result.reason.c_str());
            else
            RCLCPP_INFO(this->get_logger(), "更新参数成功");
        }
    }

private:
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Client<Patrol>::SharedPtr patrol_client_;

};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<PatrolClient>();
  node->update_server_param_k(4.0);
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}


