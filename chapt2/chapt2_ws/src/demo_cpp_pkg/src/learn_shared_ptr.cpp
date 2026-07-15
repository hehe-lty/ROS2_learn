#include "iostream"
#include "memory"

int main()
{
    //std::make_shared<数据类型/类>(参数)；返回值，对应类的共享指针 
    //std::shared_ptr<std::string> = auto;
    auto p1 = std::make_shared<std::string>("这是一个字符串");

    /*use_count()用法：
    调用方式：智能指针对象.use_count()，无入参
    主要作用：返回当前 shared_ptr 管理的堆内存，被多少个 shared_ptr 共同持有，也就是共享引用计数的实时数值。*/

    /*get()用法：
    调用方式：智能指针对象.get()，无入参
    主要作用：获取智能指针内部封装的原始裸指针（也就是被管理的堆内存的首地址），
    主要用于兼容只接受原始指针的 C 语言接口、旧版第三方库。*/

    std::cout << "p1的引用计数为:" <<p1.use_count()<< ",p1指向的内存地址为:" << p1.get() << std::endl; // 1

    auto p2 = p1;
    std::cout << "p1的引用计数为:" <<p1.use_count()<< ",p1指向的内存地址为:" << p1.get() << std::endl; // 2
    std::cout << "p2的引用计数为:" <<p2.use_count()<< ",p2指向的指针为:" << p2.get() << std::endl; // 2

    p1.reset(); //释放引用，不指向 "这是一个字符串" 所在的内存
    std::cout << "p1的引用计数为:" <<p1.use_count()<< ",p1指向的内存地址为:" << p1.get() << std::endl; // 0
    std::cout << "p2的引用计数为:" <<p2.use_count()<< ",p2指向的指针为:" << p2.get() << std::endl; // 1

    std::cout << "p2指向内存地址数据为:" << p2->c_str() << std::endl; //调用成员的方法
   
    return 0;
}