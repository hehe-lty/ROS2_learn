import launch
import launch_ros
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    #获取默认的功能包描述文件路径
    urdf_package_path = get_package_share_directory('fishbot_description')
    default_xacro_path = os.path.join(urdf_package_path, 'urdf', 'fishbot/fishbot.urdf.xacro')
    # default_rviz_config_path = os.path.join(urdf_package_path, 'config', 'display_robot.rviz')
    default_gazebo_world_config_path = os.path.join(urdf_package_path, 'world', 'custom_room.world')
    #声明一个urdf目录的参数，方便修改
    action_declare_arg_urdf_path = launch.actions.DeclareLaunchArgument(
        name='urdf_path',
        default_value=default_xacro_path,
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

    action_launch_gazebo = launch.actions.IncludeLaunchDescription(
        launch.launch_description_sources.PythonLaunchDescriptionSource([get_package_share_directory(
            'gazebo_ros'), '/launch', '/gazebo.launch.py']),
        launch_arguments=[('world', default_gazebo_world_config_path),('verbose','true')]
    )

    # 请求 Gazebo 加载机器人
    spawn_entity_node = launch_ros.actions.Node(
        package='gazebo_ros',
        executable='spawn_entity.py',
        arguments=['-topic', '/robot_description',
                   '-entity', 'fishbot', ])
    
    #joint_state_publisher节点用于发布关节状态信息，默认使用joint_state_publisher_gui图形界面
    # action_joint_state_publisher = launch_ros.actions.Node(
    #     package='joint_state_publisher',
    #     executable='joint_state_publisher',
    #     name='joint_state_publisher',
    #     output='screen'
    # )
    #启动rviz2可视化工具
    # action_rviz2_node = launch_ros.actions.Node(
    #     package='rviz2',
    #     executable='rviz2',
    #     name='rviz2',
    #     output='screen',
    #     arguments=['-d', default_rviz_config_path]
    # )

    # 加载并激活 fishbot_joint_state_broadcaster 控制器
    load_joint_state_controller = launch.actions.ExecuteProcess(
        cmd=['ros2', 'control', 'load_controller', '--set-state', 'active',
            'fishbot_joint_state_broadcaster'],
        output='screen'
    )

    # 加载并激活 fishbot_effort_controller 控制器
    load_fishbot_effort_controller = launch.actions.ExecuteProcess(
        cmd=['ros2', 'control', 'load_controller', '--set-state', 'active','fishbot_effort_controller'], 
        output='screen')
    
    load_fishbot_diff_drive_controller = launch.actions.ExecuteProcess(
        cmd=['ros2', 'control', 'load_controller', '--set-state', 'active','fishbot_diff_drive_controller'], 
        output='screen')

    return launch.LaunchDescription([
        action_declare_arg_urdf_path,
        action_robot_state_publisher,
        action_launch_gazebo,
        spawn_entity_node,
        # action_joint_state_publisher,
        # action_rviz2_node,
        # 事件动作，当加载机器人结束后执行    
        launch.actions.RegisterEventHandler(
            event_handler=launch.event_handlers.OnProcessExit(
                target_action=spawn_entity_node,
                on_exit=[load_joint_state_controller],)
            ),
        # 事件动作，load_fishbot_diff_drive_controller
        launch.actions.RegisterEventHandler(
        event_handler=launch.event_handlers.OnProcessExit(
            target_action=load_joint_state_controller,
            on_exit=[load_fishbot_diff_drive_controller],)
            ),
    ])
