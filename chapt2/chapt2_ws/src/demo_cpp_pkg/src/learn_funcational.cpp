#include <iostream>
#include <functional>

// 自由函数

void save_with_free_fun(const std::string &file_name)
{
    std::cout << "自由函数: " << file_name << std::endl;
}

// 成员函数
class FileSave
{
private:
    /* data */
public:
    FileSave(/* args */) = default;
    ~FileSave() = default;

    void save_with_member_fun(const std::string &file_name)
    {
        std::cout << "成员函数: " << file_name << std::endl;
    }
};


int main()
{

    FileSave file_save;
    //Lambda函数
    auto save_with_lambda_fun = [](const std::string &file_name) -> void
    {
        std::cout << "Lambda函数: " << file_name << std::endl;
    };

    save_with_free_fun("file.txt");
    file_save.save_with_member_fun("file.txt");
    save_with_lambda_fun("file.txt");

    
/********************将函数包装*****************************/
    // 可以绑定自由函数
    std::function<void(const std::string &)> save1 = save_with_free_fun; 
    // 可以绑定Lambda函数
    std::function<void(const std::string &)> save2 = save_with_lambda_fun; 
    
    //成员函数，放入包装器
    /************************************************************
    std::function<返回值类型(参数1类型, 参数2类型, ...)> 包装器变量 = 
    std::bind(
        &类名::成员函数名,   // 1. 成员函数指针，必须带 & 和类作用域
        对象地址/对象实例,    // 2. 调用该函数的目标对象
        std::placeholders::_1, // 3. 占位符，依次对应包装器的入参
        std::placeholders::_2,
        ...
    );*********************************************************/
    std::function<void(const std::string &)> save3 = 
    std::bind(&FileSave::save_with_member_fun, &file_save, std::placeholders::_1);

    std::cout << "*******函数已经被包装*******" << std::endl;
    //实现了统一的调用方法
    save1("file.txt");
    save2("file.txt");
    save3("file.txt");

    return 0;
}
