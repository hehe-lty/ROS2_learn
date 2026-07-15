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

