from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    static_tf_broadcaster = Node(
        package='demo_cpp_tf',
        executable='static_tf_broadcaster',
        name='static_tf_broadcaster',
        output='screen',
    )

    dynamic_tf_broadcaster = Node(
        package='demo_cpp_tf',
        executable='dynamic_tf_broadcaster',
        name='tf_broadcaster',
        output='screen',
    )

    tf_listener = Node(
        package='demo_cpp_tf',
        executable='tf_listener',
        name='tf_listener',
        output='screen',
    )

    return LaunchDescription([
        static_tf_broadcaster,
        dynamic_tf_broadcaster,
        tf_listener,
    ])
