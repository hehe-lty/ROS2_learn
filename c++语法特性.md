# C++ 现代特性使用指南

> auto、Lambda、智能指针、std::function、移动语义、右值引用、结构化绑定、std::optional/variant/string_view、并发编程

---

## 目录

1. [auto 自动类型推导](#1-auto-自动类型推导)
2. [Lambda 匿名函数](#2-lambda-匿名函数)
3. [智能指针](#3-智能指针)
4. [std::function 函数包装器](#4-stdfunction-函数包装器)
5. [移动语义与右值引用](#5-移动语义与右值引用)
6. [结构化绑定](#6-结构化绑定)
7. [std::optional / std::variant / std::string_view](#7-stdoptional-stdvariant-stdstring_view)
8. [并发编程](#8-并发编程)
9. [综合示例](#9-综合示例)

---

## 1. auto 自动类型推导

`auto` 让编译器在编译期自动推导变量类型，减少冗余代码，提高可读性。

### 1.1 基本用法

```cpp
// 基本类型推导
auto a = 42;           // int
auto b = 3.14;         // double
auto c = 'A';          // char
auto d = std::string("hello"); // std::string

// 推导时自动去掉 const 和引用
const int ci = 42;
auto x = ci;           // x 是 int（const 被去掉）
auto& y = ci;          // y 是 const int&（引用保留 const）

// 用 auto& 保持原始类型
const int& cref = ci;
auto& z = cref;        // z 是 const int&
```

### 1.2 迭代器（最常用场景）

```cpp
#include <vector>
#include <map>

std::vector<int> vec = {1, 2, 3, 4, 5};
std::map<std::string, int> mmap = {{"a", 1}, {"b", 2}};

// C++98 写法（冗长）
for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); ++it) {
    std::cout << *it;
}

// auto 写法（简洁）
for (auto it = vec.begin(); it != vec.end(); ++it) {
    std::cout << *it;
}

// 范围 for（更简洁，C++11）
for (auto elem : vec) {          // 值拷贝
    std::cout << elem;
}
for (const auto& elem : vec) {   // 推荐：避免拷贝，防止修改
    std::cout << elem;
}

// map 遍历
for (const auto& [key, value] : mmap) {  // C++17 结构化绑定
    std::cout << key << ": " << value;
}
```

### 1.3 配合 Lambda 和复杂类型

```cpp
#include <functional>

// 函数对象类型冗长，用 auto 简化
auto cmp = [](int a, int b) { return a > b; };  // 省略了 lambda 的闭包类型

// 模板函数中推导复杂类型
template<typename T, typename U>
auto multiply(T a, U b) -> decltype(a * b) {     // 尾随返回类型
    return a * b;
}
```

### 1.4 注意事项

```cpp
// 必须初始化（编译器需要初始值来推导类型）
auto x;           // 错误！没有初始值
auto y = 42;      // 正确

// 括号 vs 花括号初始化
auto a = (1, 2);  // a 是 int，值为 2（逗号表达式结果）
auto b = {1, 2};  // b 是 std::initializer_list<int>

// 每次赋值不会改变类型
auto c = 1;       // c 是 int
c = 3.14;         // c 仍是 int，赋值为 3（截断）
```

### 1.5 auto 作为函数参数（C++20）

```cpp
// C++20 起，auto 可用于函数参数（泛型 lambda 的扩展）
auto add = [](auto a, auto b) {
    return a + b;
};
add(1, 2);        // int
add(1.5, 2.5);    // double
```

---

## 2. Lambda 匿名函数

Lambda 允许在不命名函数的情况下定义匿名函数对象，常用于算法回调、事件处理等场景。

### 2.1 基本语法

```
[捕获列表] (参数列表) -> 返回类型 { 函数体 }
```

```cpp
// 最简单的 lambda
[]() { std::cout << "hello"; };

// 完整写法
auto f = [](int a, int b) -> int { return a + b; };
f(3, 4);  // 返回 7

// 返回类型可省略（编译器能推导时）
auto g = [](int a, int b) { return a + b; };
```

### 2.2 捕获列表

```cpp
int x = 10;
int y = 20;

// 值捕获（副本）
[=]() { return x + y; };              // 所有外部变量按值捕获
[x, y]() { return x + y; };           // 指定变量按值捕获

// 引用捕获（可修改外部变量）
[&]() { x++; y++; };                  // 所有外部变量按引用捕获
[&x, &y]() { x++; y++; };            // 指定变量按引用捕获

// 混合捕获
[=, &y]() { x = 5; y = 10; };        // x 值捕获，y 引用捕获
[&, x]() { x = 5; y = 10; };         // x 值捕获，y 引用捕获

// 按值捕获 this（C++17）
[this]() { std::cout << count; };
```

### 2.3 标准库算法中的 Lambda

```cpp
#include <algorithm>
#include <vector>
#include <numeric>
#include <iostream>

std::vector<int> v = {1, 2, 3, 4, 5};

// 排序（降序）
std::sort(v.begin(), v.end(), [](int a, int b) {
    return a > b;
});

// 查找第一个大于 3 的元素
auto it = std::find_if(v.begin(), v.end(), [](int n) {
    return n > 3;
});

// 统计偶数个数
int count = std::count_if(v.begin(), v.end(), [](int n) {
    return n % 2 == 0;
});

// 遍历
std::for_each(v.begin(), v.end(), [](int n) {
    std::cout << n << " ";
});

// 所有元素求和
int sum = std::accumulate(v.begin(), v.end(), 0, [](int total, int n) {
    return total + n;
});
```

### 2.4 泛型 Lambda（C++14）

```cpp
// 用 auto 实现泛型参数
auto add = [](auto a, auto b) { return a + b; };
add(1, 2);           // int: 3
add(1.5, 2.5);       // double: 4.0
add("hi", "!");      // string: "hi!"

// 遍历任意容器
auto print = [](const auto& container) {
    for (const auto& elem : container) {
        std::cout << elem << " ";
    }
};
print(std::vector{1, 2, 3});
print(std::list{'a', 'b', 'c'});
```

### 2.5 捕获 this 与异步

```cpp
class Counter {
    int count = 0;
public:
    void run() {
        // 捕获 this，可以在 lambda 中访问成员
        auto task = [this]() {
            count++;
            std::cout << count;
        };
        task();
    }
};
```

---

## 3. 智能指针

智能指针通过 RAII 机制自动管理动态内存，防止内存泄漏。

### 3.1 std::unique_ptr — 独占所有权

同一时刻只能有一个 `unique_ptr` 指向对象，**不能复制，只能移动**。

```cpp
#include <memory>

// 创建（优先使用 make_unique）
std::unique_ptr<int> p1 = std::make_unique<int>(42);
std::unique_ptr<std::vector<int>> p2 = std::make_unique<std::vector<int>>(10);

// 移动所有权（源指针变为空）
std::unique_ptr<int> p3 = std::move(p1);  // p1 现在为空
// std::unique_ptr<int> p4 = p1;          // 错误！不能复制

// 访问
*p3;           // 解引用
p3->size();    // 调用方法

// 检查是否为空
if (p3) { /* 不为空 */ }
if (!p3) { /* 为空 */ }

// 释放控制权
int* raw = p3.release();  // 返回裸指针，需手动 delete
delete raw;

// 自定义删除器
std::unique_ptr<FILE, decltype(&fclose)> fp(
    fopen("test.txt", "r"), fclose
);
```

### 3.2 std::shared_ptr — 共享所有权

多个 `shared_ptr` 共享同一对象，通过引用计数自动管理。

```cpp
#include <memory>

// 创建（优先使用 make_shared）
std::shared_ptr<int> p1 = std::make_shared<int>(42);
std::shared_ptr<int> p2 = p1;       // 引用计数 +1

// 访问（和 unique_ptr 一样）
*p1;
p1->size();

// 查询
p1.use_count();    // 当前引用计数
p1.unique();       // 引用计数是否为 1

// 减少引用
p2.reset();        // 引用计数 -1，若为 0 则释放对象

// 获取裸指针
int* raw = p1.get();
```

### 3.3 std::weak_ptr — 弱引用（不控制生命周期）

配合 `shared_ptr` 使用，观察对象但不增加引用计数，用于**打破循环引用**。

```cpp
std::shared_ptr<int> sp = std::make_shared<int>(42);
std::weak_ptr<int> wp = sp;

// 必须先升级为 shared_ptr 才能使用
if (auto locked = wp.lock()) {
    // locked 是 shared_ptr，对象仍然存活
    std::cout << *locked;
} else {
    std::cout << "对象已被释放";
}
```

### 3.4 循环引用问题

```cpp
// 错误示例：循环引用导致内存泄漏
struct NodeA {
    std::shared_ptr<class NodeB> b;
};
struct NodeB {
    std::shared_ptr<NodeA> a;   // A 和 B 互相持有 shared_ptr，引用计数永不归零
};

// 正确做法：用 weak_ptr 打破循环
struct NodeA2 {
    std::shared_ptr<class NodeB2> b;
};
struct NodeB2 {
    std::weak_ptr<NodeA2> a;    // 不增加引用计数
};
```

### 3.5 选择指南

| 场景 | 推荐 |
|------|------|
| 独占对象所有权 | `unique_ptr` |
| 多个所有者共享对象 | `shared_ptr` |
| 观察对象、避免循环引用 | `weak_ptr` |
| 函数参数（不转移所有权） | 裸指针或引用 |

---

## 4. std::function 函数包装器

`std::function` 是一个通用的多态函数包装器，可以存储、包装和调用任意可调用对象。

### 4.1 基本用法

```cpp
#include <functional>
#include <iostream>

// 包装普通函数
int add(int a, int b) { return a + b; }
std::function<int(int, int)> f1 = add;
std::cout << f1(3, 4);  // 7

// 包装 Lambda
std::function<int(int, int)> f2 = [](int a, int b) { return a * b; };
std::cout << f2(3, 4);  // 12

// 包装函数对象
struct Multiply {
    int operator()(int a, int b) const { return a * b; }
};
std::function<int(int, int)> f3 = Multiply{};
std::cout << f3(3, 4);  // 12
```

### 4.2 回调与延迟执行

```cpp
// 注册回调
void register_callback(std::function<void()> callback) {
    // 某个时刻执行
    callback();
}

register_callback([]() {
    std::cout << "回调执行了";
});

// 延迟执行
std::function<int()> create_task() {
    int counter = 0;
    return [counter]() mutable {
        return ++counter;
    };
}
auto task = create_task();
std::cout << task();  // 1
std::cout << task();  // 2
```

### 4.3 成员函数包装

```cpp
class Calculator {
public:
    int add(int a, int b) { return a + b; }
    static int multiply(int a, int b) { return a * b; }
};

Calculator calc;

// 绑定成员函数（需要绑定对象）
std::function<int(int, int)> f1 = std::bind(&Calculator::add, &calc, std::placeholders::_1, std::placeholders::_2);
std::cout << f1(3, 4);  // 7

// 更简洁的 lambda 方式
std::function<int(int, int)> f2 = [&calc](int a, int b) { return calc.add(a, b); };
std::cout << f2(3, 4);  // 7

// 静态成员函数（直接包装）
std::function<int(int, int)> f3 = Calculator::multiply;
std::cout << f3(3, 4);  // 12
```

### 4.4 与 std::bind 对比

```cpp
#include <functional>

int multiply(int a, int b) { return a * b; }

// 旧方式：std::bind
auto f1 = std::bind(multiply, 2, std::placeholders::_1);
f1(5);  // 10

// 新方式：lambda（更直观，推荐）
auto f2 = [](int b) { return multiply(2, b); };
f2(5);  // 10
```

### 4.5 注意事项

```cpp
// std::function 有少量运行时开销（类型擦除）
// 性能敏感场景考虑用模板代替

// 空 check
std::function<int(int)> f;
if (!f) {
    std::cout << "未绑定可调用对象";
}

// 类型必须匹配
std::function<int(int, int)> f2 = [](double a, double b) { return 0; };  // 错误！参数类型不匹配
```

---

## 5. 移动语义与右值引用

C++11 引入的移动语义避免了不必要的深拷贝，大幅提升性能。

### 5.1 左值 vs 右值

```cpp
int a = 10;           // a 是左值（有名字，可取地址）
int b = a + 5;        // a + 5 是右值（临时值，无名字）

std::string s1 = "hello";       // s1 是左值
std::string s2 = std::string("world");  // std::string("world") 是右值
```

### 5.2 右值引用 `&&`

```cpp
// 右值引用只能绑定到右值（临时对象）
int&& r1 = 42;                    // 正确：42 是右值
int&& r2 = r1;                    // 错误：r1 是左值（有名字）

std::string s = "hello";
std::string&& r3 = s + "world";   // 正确：s + "world" 是临时对象
```

### 5.3 std::move

`std::move` 将左值强制转换为右值引用，触发移动语义。

```cpp
#include <utility>
#include <string>
#include <vector>

std::string s1 = "hello world";
std::string s2 = std::move(s1);  // 移动而非拷贝

// s1 的内容被"偷走"，变为未指定状态（但可安全析构）
// s2 直接接管了 s1 的内存，无需重新分配

std::vector<int> v1 = {1, 2, 3, 4, 5};
std::vector<int> v2 = std::move(v1);  // 移动 vector，O(1) 而非 O(n)
```

### 5.4 移动构造函数与移动赋值

```cpp
class Buffer {
    int* data_;
    size_t size_;
public:
    // 移动构造函数
    Buffer(Buffer&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    // 移动赋值
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    // 拷贝构造（仍需保留）
    Buffer(const Buffer& other)
        : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
    }

    ~Buffer() {
        delete[] data_;
    }
};
```

### 5.5 std::forward — 完美转发

```cpp
#include <utility>

template<typename T>
void process(T&& arg) {
    // T&& + 引用折叠 = 转发引用（forwarding reference）
    // 能保留参数的 const 和引用属性

    helper(std::forward<T>(arg));  // 完美转发给 helper
}

// std::forward 根据类型推导决定是否转为右值
void helper(int& x) { std::cout << "左值"; }
void helper(int&& x) { std::cout << "右值"; }

process(42);    // 输出: 右值
int x = 10;
process(x);     // 输出: 左值
```

### 5.6 移动语义的实际收益

```cpp
std::vector<std::string> v1;
v1.push_back(std::string("hello"));  // 拷贝构造 string

std::vector<std::string> v2;
v2.push_back(std::move(std::string("hello")));  // 移动构造，避免分配
```

---

## 6. 结构化绑定

C++17 允许将复合类型的多个成员直接解包为独立变量。

```cpp
#include <tuple>
#include <map>
#include <utility>

// tuple 解包
auto person = std::make_tuple("Alice", 30, 175.5);
const auto& [name, age, height] = person;
std::cout << name << " " << age << " " << height;

// map 遍历（C++17）
std::map<std::string, int> scores = {{"Alice", 90}, {"Bob", 85}};
for (const auto& [key, value] : scores) {
    std::cout << key << ": " << value;
}

// pair 解包
std::pair<std::string, int> p = {"hello", 42};
const auto& [str, num] = p;

// 直接修改
auto [s, n] = p;
s = "world";
n = 100;

// 忽略不需要的成员
auto [name2, , height2] = person;  // 用空位忽略 age

// 数组解包
int arr[] = {1, 2, 3, 4, 5};
auto [a, b, c, d, e] = arr;
```

---

## 7. std::optional / std::variant / std::string_view

### 7.1 std::optional — 可能为空的值

```cpp
#include <optional>
#include <string>

// 查找函数：找不到时返回空值
std::optional<std::string> find_user(int id) {
    if (id == 1) return "Alice";
    return std::nullopt;  // 表示"无结果"
}

auto result = find_user(2);
if (result) {
    std::cout << *result;       // 解引用
} else {
    std::cout << "未找到";
}

// C++17 结构化绑定
if (auto user = find_user(1)) {
    std::cout << *user;
}

// value_or 提供默认值
std::string name = find_user(2).value_or("Unknown");

// has_value() / value() / reset()
result.has_value();   // 检查是否有值
result.value();       // 无值时抛 std::bad_optional_access
result.reset();       // 清空值
```

### 7.2 std::variant — 类型安全的联合

```cpp
#include <variant>
#include <string>

// variant 可以保存多种类型之一
std::variant<int, double, std::string> value;

value = 42;             // 保存 int
value = 3.14;           // 保存 double
value = "hello";        // 保存 string

// 访问值
int i = std::get<int>(value);           // 类型不匹配抛异常
std::string s = std::get<std::string>(value);

// 安全访问
if (auto* p = std::get_if<std::string>(&value)) {
    std::cout << *p;
}

// 检查当前类型
std::holds_alternative<int>(value);     // true/false

// std::visit 处理不同情况（C++17）
std::visit([](auto&& arg) {
    std::cout << arg;
}, value);
```

### 7.3 std::string_view — 只读字符串视图

```cpp
#include <string_view>

// 不拷贝字符串，只借用视图
std::string_view sv("hello world");

// 高效截取子串（不分配新内存）
std::string_view sub = sv.substr(0, 5);  // "hello"

// 作为函数参数（避免 string 拷贝）
void process(std::string_view str) {
    // 可以接收 string、string_literal、string_view
    // 不会拷贝数据
}

process(std::string("hello"));   // 不拷贝
process("hello");                // 不拷贝
std::string s = "world";
process(s);                      // 不拷贝
```

---

## 8. 并发编程

C++11 起标准库提供了完整的并发支持。

### 8.1 std::thread

```cpp
#include <thread>
#include <iostream>

void worker(int id) {
    std::cout << "线程 " << id << " 运行中\n";
}

int main() {
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);

    t1.join();   // 等待 t1 完成
    t2.join();   // 等待 t2 完成

    // 或使用 detach（分离，主线程不等待）
    // t.detach();

    return 0;
}
```

### 8.2 std::mutex — 互斥锁

```cpp
#include <mutex>
#include <thread>
#include <vector>

std::mutex mtx;
int counter = 0;

void safe_increment() {
    // 手动加锁/解锁（不推荐，可能异常不安全）
    mtx.lock();
    counter++;
    mtx.unlock();

    // 推荐：lock_guard（RAII 自动解锁）
    std::lock_guard<std::mutex> lock(mtx);
    counter++;

    // scoped_lock（C++17，多锁安全加锁）
    std::scoped_lock lock(mtx1, mtx2);
}
```

### 8.3 std::atomic — 原子操作

```cpp
#include <atomic>
#include <thread>

std::atomic<int> counter(0);

void atomic_increment() {
    counter++;  // 原子操作，无需 mutex
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back(atomic_increment);
    }
    for (auto& t : threads) {
        t.join();
    }
    std::cout << counter;  // 10
}
```

### 8.4 std::future / std::async — 异步任务

```cpp
#include <future>
#include <thread>

// async 自动创建线程并返回 future
std::future<int> result = std::async(std::launch::async, []() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 42;
});

// 获取结果（阻塞直到结果可用）
int value = result.get();  // 42

// 非阻塞检查
if (result.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
    std::cout << result.get();
}
```

### 8.5 生产者-消费者模型

```cpp
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

template<typename T>
class ThreadSafeQueue {
    std::queue<T> queue_;
    mutable std::mutex mtx_;
    std::condition_variable cv_;
public:
    void push(T item) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            queue_.push(std::move(item));
        }
        cv_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }
};
```

### 8.6 线程池（简化版）

```cpp
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool {
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stop_ = false;
public:
    ThreadPool(size_t n) {
        for (size_t i = 0; i < n; i++) {
            workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mtx_);
                        cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                        if (stop_ && tasks_.empty()) return;
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }

    template<typename F>
    auto submit(F&& f) -> std::future<decltype(f())> {
        using RetType = decltype(f());
        auto task = std::make_shared<std::packaged_task<RetType()>>(std::forward<F>(f));
        std::future<RetType> result = task->get_future();
        {
            std::lock_guard<std::mutex> lock(mtx_);
            tasks_.emplace([task]() { (*task)(); });
        }
        cv_.notify_one();
        return result;
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& w : workers_) w.join();
    }
};
```

---

## 9. 综合示例

四种核心特性协同工作的完整示例：

```cpp
#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <string>
#include <numeric>

// 可调用对象类型
using Task = std::function<void()>;

class TaskManager {
    std::vector<Task> tasks_;          // auto + std::function
public:
    // 注册任务（lambda + std::function）
    void add_task(std::function<void()> task) {
        tasks_.push_back(std::move(task));
    }

    // 执行所有任务
    void run_all() {
        for (const auto& task : tasks_) {  // auto + range for
            task();
        }
    }

    // 使用智能指针管理动态创建的任务
    void add_managed_task() {
        auto resource = std::make_unique<std::string>("任务资源");
        add_task([resource = std::move(resource)]() mutable {
            std::cout << "使用资源: " << *resource << "\n";
        });
    }
};

int main() {
    TaskManager manager;

    // Lambda + std::function：注册简单任务
    manager.add_task([]() {
        std::cout << "任务 1 执行\n";
    });

    // Lambda 捕获 + auto：注册带状态的任务
    int counter = 0;
    manager.add_task([&counter]() {
        std::cout << "计数器: " << ++counter << "\n";
    });

    // 智能指针 + lambda：资源自动管理
    manager.add_managed_task();

    // auto + range for：遍历执行
    std::cout << "=== 执行所有任务 ===\n";
    manager.run_all();

    return 0;
}
```

**输出：**
```
=== 执行所有任务 ===
任务 1 执行
计数器: 1
使用资源: 任务资源
```

---

## 快速参考

| 特性 | 核心用途 | 关键语法 |
|------|---------|---------|
| `auto` | 类型推导，简化代码 | `auto x = 42;` |
| Lambda | 匿名函数，回调 | `[=](int a){ return a; }` |
| `unique_ptr` | 独占所有权 | `make_unique<T>()` |
| `shared_ptr` | 共享所有权 | `make_shared<T>()` |
| `weak_ptr` | 弱引用，防循环 | `wp.lock()` |
| `std::function` | 函数包装器 | `std::function<R(Args)>` |
| `std::move` | 移动语义，避免拷贝 | `std::move(obj)` |
| `std::forward` | 完美转发 | `std::forward<T>(arg)` |
| 结构化绑定 | 解包复合类型 | `auto& [a, b] = tuple;` |
| `std::optional` | 可能为空的值 | `return std::nullopt;` |
| `std::variant` | 类型安全的联合 | `std::get<int>(v);` |
| `std::string_view` | 只读字符串视图 | 不拷贝字符串 |
| `std::thread` | 多线程 | `t.join();` |
| `std::mutex` | 互斥锁 | `lock_guard<mutex>` |
| `std::atomic` | 原子操作 | `counter++` |
| `std::future` | 异步结果 | `result.get();` |