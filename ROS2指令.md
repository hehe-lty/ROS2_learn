### ROS2 功能包命令


```bash
# 创建 C++ 功能包（ament_cmake 构建，Apache-2.0 许可证）
ros2 pkg create --build-type ament_cmake --license Apache-2.0 demo_cpp_pk

# 创建 Python 功能包（ament_python 构建，Apache-2.0 许可证）
ros2 pkg create demo_python_pkg --build-type ament_python --license Apache-2.0

#编译包
colcon build

#建立工作空间
mkdir -p chapt2_ws/src

#构建一个功能包
colcon build --packages-select demo_cpp_pkg

#查看节点列表
ros2 node list

#查看节点信息
ros2 node info /demo_cpp_node

#查看所有节点
ros2 topic list

#查看话题内容
ros2 topic echo /demo_cpp_node/chapt2_topic

#查看话题信息
ros2 topic info /demo_cpp_node/chapt2_topic

#查看消息接口列表
ros2 interface list

#查看消息接口详细定义
ros2 internface show std_msgs/msg/String

#查看话题发布者
ros2 topic echo /demo_cpp_node/chapt2_topic

#查看话题订阅者
ros2 topic echo /demo_cpp_node/chapt2_topic

#查看话题发布者
ros2 topic echo /demo_cpp_node/chapt2_topic

#查看话题订阅者
ros2 topic echo /demo_cpp_node/chapt2_topic

#查看话题发布者
ros2 topic echo /demo_cpp_node/chapt2_topic

#通过命令行发布消息ros2 topic pub 话题 消息类型 '{字段: 值, 字段: 值}'
ros2 topic pub /turtle1/cmd_vel geometry_msgs/msg/Twist '{linear: {x: 2.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 1.8}}'

#查看话题频率
ros2 topic hz /turtle1/cmd_vel 

#查看参数列表
ros2 param list

#查询参数含义
ros2 param describe /turtlesim background_r

#查询参数值
ros2 param get /turtlesim background_r

#设置参数值
ros2 param set /turtlesim background_r 200

#将参数导出到文件里面
ros2 param dump /turtlesim > turtlesim_param.yaml

#将参数文件导入到程序中
ros2 run turtlesim turtlesim_node --ros-args --params-file turtlesim_param.yaml 





