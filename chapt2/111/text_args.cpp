#include"iostream"

int main(int argc, char** argv) {

    std::cout << "参数数量="<<argc<< std::endl;
    std::cout << "程序名字="<<argv[0]<< std::endl;
    std::string arg1 = argv[1];
    if(arg1 == "--help")
    {
        std::cout << "使用方法：" << std::endl;
        std::cout << "  ros2 run ros2_cpp_node [--help]" << std::endl;
        std::cout << "  --help: 显示帮助信息" << std::endl;
    }
    return 0;
}