import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/rick/ROS2/chapt3/chapt3_ws/install/demo_python_topic'
