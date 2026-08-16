import launch
import launch_ros
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    #获取默认的机器人描述文件路径
    urdf_package_path = get_package_share_directory('fishbot_description')
    default_urdf_path = os.path.join(urdf_package_path, 'urdf', 'first_robot.xacro')
    default_rviz_config_path = os.path.join(urdf_package_path, 'config', 'display_robot.rviz')
    #声明一个urdf目录的参数，方便修改
    action_declare_arg_urdf_path = launch.actions.DeclareLaunchArgument(
        name='urdf_path',
        default_value=default_urdf_path,
        description='加载的模型文件路径'
    )
    #通过文件路径获取内容，并转换为参数值对象，以供传入robot_state_publisher节点
    substitutions_command = launch.substitutions.Command(['xacro ', launch.substitutions.LaunchConfiguration('urdf_path')])
    robot_description_value = launch_ros.parameter_descriptions.ParameterValue(substitutions_command, value_type=str)

    action_robot_state_publisher = launch_ros.actions.Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{'robot_description': robot_description_value}]
    )
    #joint_state_publisher节点用于发布关节状态信息，默认使用joint_state_publisher_gui图形界面
    action_joint_state_publisher = launch_ros.actions.Node(
        package='joint_state_publisher',
        executable='joint_state_publisher',
        name='joint_state_publisher',
        output='screen'
    )
    #启动rviz2可视化工具
    action_rviz2_node = launch_ros.actions.Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', default_rviz_config_path]
    )

    return launch.LaunchDescription([
        action_declare_arg_urdf_path,
        action_robot_state_publisher,
        action_joint_state_publisher,
        action_rviz2_node,
    ])
