#include"rclcpp/rclcpp.hpp"

int main(int argc, char** argv) { //argc,用于获取命令行参数个数，argv，用于获取命令行参数

    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("cpp_node");
    RCLCPP_INFO(node->get_logger(), "你好cpp!");
    rclcpp::spin(node);
    rclcpp::shutdown();
    
    return 0;
}
