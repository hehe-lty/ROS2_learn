import launch
import launch_ros
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    #  动作1，启动其他launch
    # multisim_launch_path = get_package_share_directory('turtlesim') + '/launch/multisim.launch.py'
    action_include_launch = launch.actions.IncludeLaunchDescription(
        launch.launch_description_sources.PythonLaunchDescriptionSource(
            [get_package_share_directory('turtlesim'), '/launch', '/multisim.launch.py']
        )
    )
    # 动作2，打印数据
    action_log_info = launch.actions.LogInfo(msg = '启动demo_cpp_service包的launch文件')

    #动作3，执行进程，其实就是执行命令行，启动节点
    action_topic_list = launch.actions.ExecuteProcess(
        cmd=['ros2', 'topic', 'list'],
        output='screen'
    )
    # 动作4，组织动作成组，就是把多个动作组织成一个动作组，方便管理
    action_group = launch.actions.GroupAction([
        # 动作5，定时器
        launch.actions.TimerAction(period=2.0, actions=[action_include_launch]),
        launch.actions.TimerAction(period=4.0, actions=[action_topic_list]),

    ])

    return launch.LaunchDescription([
        #action动作
        # action_include_launch,
        # action_topic_list,
        action_log_info,
        action_group,
    ])