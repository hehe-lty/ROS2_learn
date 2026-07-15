# ROS 2 常用命令大全

> 涵盖核心命令、包管理、调试、工具链等常用操作

---

## 目录

1. [基础命令](#1-基础命令)
2. [包管理](#2-包管理)
3. [编译构建](#3-编译构建)
4. [节点与话题](#4-节点与话题)
5. [服务与动作](#5-服务与动作)
6. [参数与日志](#6-参数与日志)
7. [消息与数据](#7-消息与数据)
8. [仿真与可视化](#8-仿真与可视化)
9. [环境配置](#9-环境配置)
10. [测试与调试](#10-测试与调试)
11. [常用场景速查](#11-常用场景速查)

---

## 1. 基础命令

### 1.1 ros2 run — 运行节点

```bash
# 运行某个包中的可执行文件
ros2 run <package_name> <executable_name>

# 示例：运行 turtlesim 包中的小海龟节点
ros2 run turtlesim turtlesim_node

# 带参数运行
ros2 run turtlesim turtlesim_node --ros-args -p background_r:=200

# 重映射名称（remap）
ros2 run turtlesim turtlesim_node --remap __node:=my_turtle
```

### 1.2 ros2 node — 节点管理

```bash
# 列出所有节点
ros2 node list

# 查看节点信息
ros2 node info /turtlesim

# 查看节点名称
ros2 node name

# 重启/杀死节点
ros2 node kill /turtlesim

# 暂停/恢复节点
ros2 node pause /turtlesim
ros2 node resume /turtlesim
```

### 1.3 ros2 pkg — 包管理

```bash
# 列出所有已安装的包
ros2 pkg list

# 列出符合模式的包
ros2 pkg list --pattern turtlesim

# 查看包的路径
ros2 pkg prefix <package_name>
# 示例：ros2 pkg prefix rclpy → /opt/ros/humble

# 查看包的依赖
ros2 pkg depends <package_name>

# 查看哪些包依赖当前包
ros2 pkg autodepends <package_name>

# 查看包的内容
ros2 pkg bin <package_name>      # 可执行文件
ros2 pkg lib <package_name>     # 库文件
ros2 pkg include <package_name> # 头文件
```

### 1.4 ros2 component — 组件管理

```bash
# 列出已加载的组件
ros2 component list

# 加载组件
ros2 component load <container_name> <package_name> <component_type>

# 卸载组件
ros2 component unload <container_name> <component_index>

# 卸载所有组件
ros2 component configure <container_name>
```

---

## 2. 编译构建

### 2.1 colcon — 构建工具

```bash
# 编译工作区（最常用）
colcon build

# 编译指定包
colcon build --packages-select <package_name>

# 编译多个包
colcon build --packages-select pkg1 pkg2

# 增量编译（只编译变化的包）
colcon build --packages-up-to <package_name>

# 查看详细输出
colcon build --event-handlers console_direct+

# 指定安装路径
colcon build --install-base /tmp/install

# 清理构建
colcon clean

# 编译并自动 source
colcon build && source install/setup.bash

# 常用构建选项
colcon build \
    --cmake-args -DCMAKE_BUILD_TYPE=Release \
    --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    --symlink-install    # 符号链接，避免拷贝源文件
```

### 2.2 工作区操作

```bash
# 创建新工作区
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src
# 克隆或创建包...

# 编译
cd ~/ros2_ws
colcon build

# 环境变量
source install/setup.bash          # bash
source install/setup.zsh           # zsh
source install/setup.fish          # fish

# 在已有环境中叠加（不覆盖原有 ROS2 命令）
source install/setup.bash --merge-only
```

---

## 3. 节点与话题

### 3.1 ros2 topic — 话题操作

```bash
# 列出所有活跃话题
ros2 topic list

# 列出符合模式的话题
ros2 topic list --pattern /turtle*

# 查看话题类型
ros2 topic type /turtle1/cmd_vel

# 查看发布在话题上的节点
ros2 topic info /turtle1/cmd_vel

# 发布消息
ros2 topic pub /turtle1/cmd_vel geometry_msgs/msg/Twist \
    '{linear: {x: 0.5, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.5}}'

# 发布一次后停止（默认）
ros2 topic pub --once /turtle1/cmd_vel geometry_msgs/msg/Twist \
    '{linear: {x: 0.5}, angular: {z: 0.5}}'

# 周期性发布（每秒 1 次）
ros2 topic pub --rate 1 /turtle1/cmd_vel geometry_msgs/msg/Twist \
    '{linear: {x: 0.5}, angular: {z: 0.5}}'

# 订阅并打印话题内容
ros2 topic echo /turtle1/pose

# 只打印特定字段
ros2 topic echo /turtle1/pose --field x

# 限制打印次数
ros2 topic echo /turtle1/pose --once

# 查看话题带宽等信息
ros2 topic bw /turtle1/pose

# 查看话题频率
ros2 topic hz /turtle1/pose

# 话题延迟监控
ros2 topic delay /turtle1/pose
```

### 3.2 ros2 service — 服务操作

```bash
# 列出所有服务
ros2 service list

# 列出符合模式的服务
ros2 service list --pattern /turtle*

# 查看服务类型
ros2 service type /turtle1/set_pen

# 查看服务的节点
ros2 service info /turtle1/set_pen

# 调用服务
ros2 service call /turtle1/set_pen turtler_interfaces/srv/SetPen \
    '{r: 255, g: 0, b: 0, width: 5, off: 0}'

# 查看服务类型后调用（更实用）
ros2 service type /turtle1/set_pen    # 先查类型
ros2 service call /turtle1/set_pen <type> '<args>'
```

### 3.3 ros2 action — 动作操作

```bash
# 列出所有动作
ros2 action list

# 查看动作类型
ros2 action type /fibonacci

# 发送动作目标
ros2 action send_goal /fibonacci action_tutorials_interfaces/action/Fibonacci \
    '{order: 5}'

# 查看动作信息
ros2 action info /fibonacci

# 取消动作
ros2 action cancel /fibonacci 1  # 1 是 goal_id
```

### 3.4 ros2 interface — 接口操作

```bash
# 列出所有消息/服务/动作接口
ros2 interface list

# 列出符合模式的接口
ros2 interface list --pattern std_msgs

# 查看接口定义
ros2 interface show std_msgs/msg/String

# 查看接口的依赖
ros2 interface show std_msgs/msg/String --dependencies

# 列出服务接口
ros2 interface list --show-srv

# 列出动作接口
ros2 interface list --show-action
```

---

## 4. 参数与日志

### 4.1 ros2 param — 参数管理

```bash
# 列出节点的所有参数
ros2 param list /turtlesim

# 查看参数值
ros2 param get /turtlesim background_g

# 设置参数值
ros2 param set /turtlesim background_g 100

# 设置参数（类型自动推断）
ros2 param set /turtlesim background_r 255

# 设置布尔参数
ros2 param set /my_node use_sim_time true

# 从文件加载参数
ros2 param load /my_node params.yaml

# 查看参数类型
ros2 param describe /turtlesim background_g

# 动态修改参数（不重启节点）
ros2 param set /my_node some_param 42

# 在 launch 文件中设置参数
# <param name="some_param" value="42"/>
```

### 4.2 ros2 log — 日志查看

```bash
# 查看日志
ros2 log

# 查看特定日志级别
ros2 log list

# 设置日志级别
ros2 log set_level rcl:DEBUG

# 查看 ROS2 日志文件
# 默认位置：~/.ros/log/
ls ~/.ros/log/
```

---

## 5. 消息与数据

### 5.1 ros2 bag — 录制与回放

```bash
# 录制所有话题
ros2 bag record -o my_bag *

# 录制指定话题
ros2 bag record -o my_bag /turtle1/pose /turtle1/cmd_vel

# 按消息内容录制
ros2 bag record -o my_bag --topic-filter '/turtle1/*'

# 录制时压缩（需要 rmw 支持）
ros2 bag record -o my_bag_compressed --compression-mode file \
    --compression-format zstd

# 回放
ros2 bag play my_bag

# 回放时指定速率
ros2 bag play my_bag --rate 0.5    # 慢放

# 回放时重映射话题
ros2 bag play my_bag --remap /turtle1/cmd_vel:/my_turtle/cmd_vel

# 回放时设置延迟
ros2 bag play my_bag --sleep 1

# 查看 bag 信息
ros2 bag info my_bag

# 列出 bag 中的话题
ros2 bag info my_bag --topics

# 将 bag 转换为 ROS 1 bag
ros2 bag fix my_bag
```

### 5.2 ros2 exec — 执行器

```bash
# 查看执行器信息
ros2 exec list

# 查看执行器详情
ros2 exec info <executor_name>
```

---

## 6. 仿真与可视化

### 6.1 ros2 run + rviz2 — 可视化

```bash
# 启动 RViz2
ros2 run rviz2 rviz2

# 启动带配置文件的 RViz2
ros2 run rviz2 rviz2 -d /path/to/config.rviz

# 启动 Gazebo
ros2 run gazebo_ros gazebo

# 启动 Gazebo 带世界文件
ros2 run gazebo_ros gazebo -- -r /path/to/world.sdf
```

### 6.2 ros2 run + turtlesim — 小海龟示例

```bash
# 启动小海龟
ros2 run turtlesim turtlesim_node

# 另一个终端：控制小海龟
ros2 topic pub /turtle1/cmd_vel geometry_msgs/msg/Twist \
    '{linear: {x: 1.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.5}}' --once

# 另一个终端：重置
ros2 service call /turtle1/reset std_srvs/srv/Empty {}

# 另一个终端：设置背景色
ros2 param set /turtlesim background_r 255
```

---

## 7. 环境配置

### 7.1 环境变量

```bash
# 查看 ROS 版本
echo $ROS_DISTRO          # humble, iron, jazzy 等
echo $ROS_HOME            # 默认 ~/.ros
echo $ROS_LOG_DIR         # 日志目录
echo $ROS_PACKAGE_PATH    # 包搜索路径

# 设置环境变量
export ROS_DOMAIN_ID=0    # 域 ID，同域才能通信
export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp

# 查看当前域 ID
echo $ROS_DOMAIN_ID
```

### 7.2 多机通信

```bash
# 主机（Master）
export ROS_DOMAIN_ID=10
ros2 run my_pkg my_node

# 从机
export ROS_DOMAIN_ID=10          # 必须相同
export ROS_HOSTNAME=192.168.1.2  # 或 ROS_IP=192.168.1.2
ros2 run my_pkg my_node
```

### 7.3 多版本 ROS2 共存

```bash
# 不同版本 source 不同 setup.bash
source /opt/ros/humble/setup.bash    # Humble
source /opt/ros/iron/setup.bash      # Iron

# 叠加使用（不冲突）
source /opt/ros/humble/setup.bash
source ~/ros2_ws/install/setup.bash
```

---

## 8. 测试与调试

### 8.1 ros2 test — 测试

```bash
# 运行测试
colcon test
colcon test --packages-select <package_name>

# 查看测试结果
colcon test-result

# 详细输出
colcon test-result --verbose

# 运行指定测试
ament test --package-select <package_name>

# 运行 pytest
pytest tests/test_file.py
```

### 8.2 ros2 doctor — 诊断

```bash
# 检查 ROS2 环境健康状态
ros2 doctor

# 检查特定组件
ros2 doctor --report
```

### 8.3 调试技巧

```bash
# 使用 gdb 调试节点
gdb --args ros2 run pkg node

# 使用 valgrind 检测内存泄漏
valgrind --leak-check=full ros2 run pkg node

# 使用 strace 跟踪系统调用
strace -f ros2 run pkg node

# 查看节点间的连接图
ros2 run rqt_graph rqt_graph

# 使用 rqt_plot 绘制数据
ros2 run rqt_plot rqt_plot

# 使用 rqt_console 查看日志
ros2 run rqt_console rqt_console

# 使用 rqt_reconfigure 动态调参
ros2 run rqt_reconfigure rqt_reconfigure
```

---

## 9. Launch 文件

### 9.1 常用 Launch 命令

```bash
# 运行 launch 文件
ros2 launch <package_name> <launch_file>.launch.py

# 示例
ros2 launch turtlesim launch_turtlesim.launch.py

# 覆盖 launch 文件中的参数
ros2 launch <pkg> <file>.launch.py some_param:=42

# 使用 launch 参数文件
ros2 launch <pkg> <file>.launch.py params_file:=/path/to/params.yaml

# 列出 launch 文件
ros2 launch <pkg> --show-xml

# 验证 launch 文件
ros2 launch <pkg> <file>.launch.py --dry-run
```

### 9.2 常用 Launch 组件

```python
# 典型的 launch 文件结构
import launch
import launch.actions
import launch_ros.actions

def generate_launch_description():
    return launch.LaunchDescription([
        # 启动节点
        launch_ros.actions.Node(
            package='turtlesim',
            executable='turtlesim_node',
            name='my_turtle',
            parameters=[{'background_r': 255}],
            remappings=[('/turtle1/cmd_vel', '/my_cmd_vel')],
            output='screen',
        ),

        # 启动多个节点
        launch_ros.actions.Node(
            package='turtlesim',
            executable='teleop_turtle_node',
        ),

        # 启动带参数文件的节点
        launch_ros.actions.Node(
            package='my_robot',
            executable='driver',
            parameters=['config.yaml'],
        ),
    ])
```

---

## 10. 常用场景速查

### 快速启动

```bash
# 1. 编译
cd ~/ros2_ws && colcon build && source install/setup.bash

# 2. 查看可用包
ros2 pkg list | grep turtlesim

# 3. 运行节点
ros2 run turtlesim turtlesim_node

# 4. 查看话题
ros2 topic list

# 5. 发布消息
ros2 topic pub /turtle1/cmd_vel geometry_msgs/msg/Twist \
    '{linear: {x: 1.0}, angular: {z: 0.5}}' --once

# 6. 录制数据
ros2 bag record -o my_bag /turtle1/pose

# 7. 回放数据
ros2 bag play my_bag
```

### 创建新包

```bash
# 在 src 目录下创建 Python 节点包
ros2 pkg create my_python_pkg --build-type ament_python \
    --package-name my_python_pkg \
    --dependencies rclpy

# 创建 C++ 节点包
ros2 pkg create my_cpp_pkg --build-type ament_cmake \
    --package-name my_cpp_pkg \
    --dependencies rclcpp geometry_msgs

# 创建包含 launch 的包
ros2 pkg create my_pkg --dependencies rclcpp rclpy \
    --node-name my_node --launch-name my_launch
```

---

## 快速参考表

| 命令 | 用途 |
|------|------|
| `ros2 run pkg exec` | 运行节点 |
| `ros2 node list` | 列出所有节点 |
| `ros2 topic list` | 列出所有话题 |
| `ros2 topic pub` | 发布话题消息 |
| `ros2 topic echo` | 订阅并打印话题 |
| `ros2 service call` | 调用服务 |
| `ros2 action send_goal` | 发送动作目标 |
| `ros2 param set/get` | 设置/查看参数 |
| `ros2 bag record/play` | 录制/回放 bag |
| `ros2 interface show` | 查看接口定义 |
| `colcon build` | 编译工作区 |
| `ros2 launch pkg file.launch.py` | 运行 launch 文件 |
| `ros2 pkg list` | 列出已安装包 |
| `ros2 pkg create` | 创建新包 |
| `ros2 doctor` | 环境诊断 |
| `source install/setup.bash` | 加载环境变量 |
| `export ROS_DOMAIN_ID=N` | 设置通信域 |