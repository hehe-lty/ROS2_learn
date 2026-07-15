#include <iostream>
#include <algorithm>

int main()
{
    // 使用lambda定义函数
    /*
    [捕获列表] (参数列表) mutable 异常说明 -> 返回值类型
    {
        函数体逻辑
    }*/
    
    auto add = [](int a, int b)
    { return a + b; };
    int sum = add(200, 50);
    auto print_sum = [sum]() -> void
    { std::cout << "Sum is " << sum << std::endl; };

    print_sum();

    return 0;
}