#include "rclcpp/rclcpp.hpp"

class PersonNode : public rclcpp::Node
{
private:
//声明
    std::string name_;
    int age_;

public:
  PersonNode(const std::string &node_name, const std::string &name,const int &age)
: Node(node_name) //调用父类的构造函数，等同于python中的 super().__init__()
  {
    this->name_ = name;//这里 name_ 是类的成员变量，name 是构造函数的入参。加 this-> 可以清晰标明「这是我这个对象的成员变量」，避免和同名参数、局部变量混淆。
    this->age_ = age;//代码里所有 this->xxx，本质都是「我这个对象自己的 xxx
    // RCLCPP_INFO(this->get_logger(), "Person node has been started.");
  }

  void eat(const std::string &food_name)
  {
    /*c_str() 就是做适配转换的：它返回一个指向字符串首字符的 const char* 指针，
    内容和原 std::string 完全一致，且末尾自动带 \0，完全符合 C 字符串的规范。
    如果用的是std::cout输出，就不需要调用c_str()，cout已经做了运算符重载，可
    以直接识别并输出 std::string 对象。*/
    
    RCLCPP_INFO(this->get_logger(), "我是%s,%d岁,我正在吃%s", this->name_.c_str(), 
                this->age_, food_name.c_str());
   
  }
};
int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<PersonNode>("person_node", "小鱼", 18);
  RCLCPP_INFO(node->get_logger(), "大家好");
  node->eat("鱼香肉丝");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}

