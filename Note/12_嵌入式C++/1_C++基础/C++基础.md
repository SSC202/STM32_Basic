# C++ 基础

---

第一个程序：

```c++
#include <iostream>

int main()
{
    // std::cout - 字符输出
    // std::endl - 输出换行符并刷新缓冲区
    // << - 重载运算符, 将右边的内容推到左边
    // std 为命名空间, 一般为避免命名冲突, 会带上 std:: 前缀而非使用 using namespace std
    
    // std::cin - 字符输入
    // >> - 重载运算符, 将左边的内容推到右边
    std::cout << "Hello, C++!" << std::endl;
    return 0;
}
```

---

## 1. 数据类型与值类别

### 数据类型

---

**整型：**

最基本的整型包括 `short`，`int`，`long`，`long long`。每种整型可以加 `unsigned` 前缀变为无符号整型。

C++ 只规定了这些整型的最小位数，但是并没有强制规定整型的位数。这意味着不同平台上的 `sizeof` 的输出可能并不相同。

对于跨平台代码，可以使用 `<cstdint>` 提供的固定宽度类型。比如 `int8_t` 指 8 位有符号整型。在平台不支持某些宽度时，使用这些宽度的整型将会使得编译报错。

---

**浮点数：**

浮点数包括 `float`（单精度 4 位），`double`（双精度 8 位）和 `long double`（扩展精度，位数因平台而异）。

`float` 可提供 7 位有效数字，`double` 可提供 15 位。

---

**字符类型：**

`char` 是 8 位数据，但是其是否有符号由平台决定。

C++ 中有一些特定的字符类型：`wchar_t` 是"宽字符"，在 Windows 上是 2 字节（UTF-16），在 Linux 上是 4 字节（UTF-32），所以它也不跨平台。C++11 引入了 `char16_t`（2 字节，对应 UTF-16）和 `char32_t`（4 字节，对应 UTF-32），C++20 又加了 `char8_t`（1 字节，对应 UTF-8）。

---

**布尔类型：**

`bool` 类型只有两个值 `true` 和 `false`，但是仍然是 8 位整型。

`bool` 和整型之间有一套隐式转换规则：零值转换为 `false`，任何非零值转换为 `true`。反过来，`false` 转换为 `0`，`true` 转换为 `1`。

---

**`sizeof`**

`sizeof` 是 C++ 的运算符，可以在编译阶段计算类型/变量占用的字节数，没有运行开销。

---

**如何选择变量类型**

1. 一般用途的整数推荐用 `int`，运算速度最快且代码生成最优化。
2. 位宽确定时，需要使用固定宽度类型。
3. 浮点数计算使用 `double`，特别是在有 FPU 的条件下。
4. 逻辑用 `bool`，可以使得代码意图更加清晰。

### 类型转换

---

**隐式转换：**

1. 整数提升：比 `int` 更小的整数类型参与运算时自动提升为 `int`。（CPU 上很多时候 `int` 是原生运算宽度，效率最高）
2. 算术转换：两个不同类型的值运算时，编译器会向大的类型靠。

> 小的类型转换为大的类型时不会带来数据损失，但是大的类型转换到小的类型会发生数据截断。

---

C 语言风格的显式类型转换一般不用。

**显式转换：**

`static_cast<目标类型>(表达式)` 可以在编译时检查并拒绝不合理操作。

`reinterpret_cast<目标类型>(表达式)` 可以把一段内存当作另一种类型看待。但是极有可能造成未定义行为，其安全的用例仅包括：`void*` 和原始指针类型之间的互转、基于 `unsigned char` 的底层字节观察，以及一些序列化和硬件寄存器访问的场景。

同时还有 `const_cast` 和 `dynamic_cast`，这里不做介绍。

### `const` 和 `constexpr`

---

通常更推荐用 `const` 替代 `#define`，`const` 变量有明确的类型；遵循正常的作用域规则；调试的时候 `const` 变量就是一个普通的变量。因此常量定义优先用 `const`。

`const int* p` - 指向常量的指针，不能通过指针修改整型常量，但是指针可以改变，同时直接修改指针指向的数据是合法的。

`int* const p` - 常量指针，不能修改指针指向的位置，可以通过指针修改目标数据。

`const int* const p` - 指向位置不变，也不能通过指针改变指向数据（但是数据本身可变，除非该数据也被 `const` 修饰）。

---

***`constexpr` 的值在编译时确定。***

如果一个函数被 `constexpr` 修饰，说明如果传入参数可在编译时确定，则返回值也可以在编译时计算。

```c++
constexpr int square(int x)
{
    return x * x;
}

constexpr int kResult = square(5);	// 编译时 kResult 已经计算完毕
```

### 值类别

左值是一个有名字有确定内存地址的表达式，可以对其取地址(`&` 运算符)，其生命周期不会在表达式结束时终止。

右值是临时产生的值，没有办法进行取地址操作。

## 2. 控制流

### 条件语句

C++17 引入了带初始化的条件语句，可以在判断开始前进行计算。

```c++
if (int x = compute_value(); x > 0) {
    std::cout << x << "> 0" << std::endl;
} else {
    std::cout << x << "< 0" << std::endl;
}
```

### 循环语句

C++ 中的 `for` 循环支持同时操作多个变量：

```c++
int data[] = {1, 2, 3, 4, 5};
int n = 5;

for (int i = 0, j = n - 1; i < j; ++i, --j) {
    int temp = data[i];
    data[i] = data[j];
    data[j] = temp;
}
```

---

**`range-for` 循环：**

`for` 循环编写时需要***维护索引***，这可能会引入不少 bug。C++11 引入的 `range-for` 循环舍弃了索引，可读性更强。

```c++
for (类型 变量名 : 集合) {
    // 使用变量
}
```

```c++
#include <iostream>

int main()
{
    int scores[] = {90, 85, 78, 92, 88};

    for (int score : scores)
    {
        // score 将会依次遍历 scores 数组的值
        std::cout << score << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

`range-for` 可以搭配 `auto`：

1. 按值访问： `for (auto x : arr)` 每次迭代时幅值一份元素到 `x`，修改 `x` 不影响 `arr`。
2. 按引用访问：`for(auto& x: arr)` 每次迭代时让 `x` 成为 `arr` 内元素的引用，修改 `x` 会修改 `arr`。
3. 按 `const` 引用访问：`for(const auto& x: arr)` 每次迭代时让 `x` 成为 `arr` 内元素的只读引用，避免拷贝开销和意外修改。

`range-for` 不能用指针作为集合，因为丢失了元素数量信息。

## 3. 函数

C++14 引入了返回类型推导，在返回类型的位置用 `auto` 替代，编译器根据 `return` 自动推导返回类型。

对于多个 `return` 语句，需要保证这些语句有同样的返回类型。

### 参数传递

---

**值传递：**

***值传递时，函数拿到实参的副本，实参不受到任何修改。***

值传递具有隔离性，但是每次调用都会进行拷贝，对于大量数据的拷贝开销较大。
```c++
#include <iostream>

auto add(int a, int b)
{	
    return a + b;
}

void main()
{
    int num = 0;
    int x = 5, y = 6;
    // x, y 被复制到 a, b 中, x, y 不会被修改
    num = add(x, y);
}
```

---

**引用传递：**

***引用传递时，函数直接访问实参，实参会被函数修改。***

参数类型后加 `&` 声明引用参数。

```c++
void swap(int& a, int& b)
{
    int temp = a;
    a = b;
    b = temp;
}
```

注意，右值不能进行引用传递，比如 `swap(3, 5);` 会直接导致编译错误。

---

**`const` 引用传递：**

***`const` 引用传递没有拷贝开销，同时实参不会被修改。***

```c++
#include <iostream>

auto add(const int& a, const int& b)
{	
    return a + b;
}

void main()
{
    int num = 0;
    int x = 5, y = 6;
    // x, y 是 a, b 的引用, x, y 不会被修改
    num = add(x, y);
}
```

函数能接收左值和右值。

---

**传参的选择：**

| 参数类型   | 不修改        | 需要修改            |
| :--------- | :------------ | :------------------ |
| 基本类型   | `T`（值传递） | `T`（值传递后返回） |
| 非平凡类型 | `const T&`    | `T&`                |

---

**返回多个值：**

C++ 推荐返回结构体而非使用输出形参：

```c++
struct DivResult {
    int quotient;
    int remainder;
};

DivResult divide(int a, int b)
{
    return {a / b, a % b};
}
```

### 重载和默认参数

---

**函数重载：**

多个函数可以共享同一个名字，只要参数列表不同 —— 参数的***类型***不同，或者参数的***数量***不同。（返回类型不在考虑范围内）

```c++
#include <cstdio>

void print(int value)
{
    std::printf("Integer: %d\n", value);
}

void print(double value)
{
    std::printf("Double: %f\n", value);
}

void print(const char* str)
{
    std::printf("String: %s\n", str);
}
```

---

**重载决议：**

重载优先级从高到低：

1. 精确匹配：实参与形参类型完全一致；
2. 类型提升：比如 `char` 提升为 `int`、`float` 提升为 `double`。
3. 标准类型转换：例如 `int` 转换为 `double`。
4. 自定义转换。

> 如果仅靠参数顺序或微妙的类型差异来区分重载，会导致歧义：比如同时存在  `func(int, double)`  和  `func(double, int)` ，并调用 `func(5, 5)` 时。

---

**默认参数：**

函数可以指定默认参数。

```c++
void configure_uart(int baudrate,
                    int databits = 8,
                    int stopbits = 1,
                    char parity = 'N')
{
    // 配置 UART
}
```

此时调用函数时，只需要传入非默认参数即可，如果多传参数，则默认参数会被传入的参数修改。

- 默认参数必须从右向左连续出现。
- ***默认参数只能被指定一次，而且应该放在声明处***。

---

**重载和默认参数：**

重载和默认参数最好二选一，这样不容易产生歧义。

### `inline` 和 `constexpr`

---

**`inline` 函数：**

 被标记为 `inline` 的函数允许在多个翻译单元中存在相同的定义，只要所有定义完全一致 —— 链接器会自动合并它们，只保留一份。

`inline` 函数的定义必须出现在头文件中，定义和声明必须一起。

---

**`constexpr` 函数：**

`constexpr` 函数在被调用时，如果所有参数都是编译期已知的常量，函数的求值会发生在编译阶段，结果直接变成一个常量。如果参数中包含运行时才能确定的值，函数就退化为普通的运行时调用。

`constexpr` 的值是 `const` 的，反之不行。

C++11 时 `constexpr` 函数体只能包含一条 `return` 语句；C++14 大幅放宽了限制，函数体里可以有局部变量、`if/else`、`for/while` 循环；C++20 更是放开了几乎所有限制。

数学函数（平方、绝对值、最大公约数）、查找表生成（正弦表、CRC 表）、配置值计算（寄存器地址、缓冲区大小）、类型特征判断（`std::size()`、`std::extent_v`）——这些不依赖运行时状态的纯计算，都应该尽量交给编译器。编译器比 CPU 更有耐心，而且只算一次。

## 4. 指针

### 空指针

C++98 和 C 中使用的空指针是 `NULL`，实际上是 `(void *)0`。C++11 引入的 `nullptr` 是表示空指针的唯一正确方式。

### 引用

引用的本质是别名：给已经存在的变量起另一个名字。

```c++
int value = 42;
int& ref = value;  // ref 是 value 的别名

ref = 100;         // 通过别名修改原变量
```

> - 引用必须在声明时就初始化；
> - 引用一旦绑定就不能换目标。

> 引用和指针：
>
> 1. 引用声明时必须绑定到一个对象，所以一个引用永远是有效的。指针则可以先声明为 `nullptr`，后面再赋值；
> 2. 引用一旦绑定就终身不变，指针随时可以指向别的对象。
> 3. 使用引用就像使用普通变量一样，直接写名字就行。指针则需要 `*ptr` 或 `ptr->member` 来访问目标。

一般使用引用，除非需要表达"可能没有对象"的概念，在运行时改变指向的目标，做指针算术来遍历内存。

---

**返回引用：**

返回引用可以返回类成员的引用，让外部代码可以直接读写内部数据；

返回引用可以实现链式调用——让函数返回 `*this` 的引用，这样调用者就能在一行代码里串联多个操作。

不要返回局部变量的引用，局部变量存储在栈上，函数返回后栈帧被回收，此时引用指向的是一块已经被释放的内存。

---

**`const` 引用**：

`const` 引用可以绑定到右值上，并且会延长这个临时对象的生命周期。

```c
const int& ref = 42;  
// ref 在整个作用域内有效，值为 42
```

```c++
std::string get_name();

const std::string& name = get_name();
// get_name() 返回的临时 string 本来在完整表达式结束后就该销毁, 但 const 引用绑定了它，生命周期被延长到 name 的作用域结束, name 在整个作用域内都是安全的
```

引用必须直接绑定到临时对象，生命延长才会生效。

## 5. 类与面向对象

C++ 的类把数据和操作数据的函数收拢到同一个语法单元里，而且允许控制哪些成员对外可见、哪些是内部实现细节。

同时 C++ 的结构体也可以包含成员函数，和类的区别是默认访问权限不同。

```c++
class Point {
private:
    double x;
    double y;

public:
    void set(double new_x, double new_y)
    {
        x = new_x;
        y = new_y;
    }

    double distance_to(const Point& other) const
    {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    void print() const
    {
        std::cout << "(" << x << ", " << y << ")";
    }
};
```

### 类的定义

- 成员变量和成员函数：

  类体内部可以包含**成员变量**（也叫数据成员，描述对象状态）和**成员函数**（也叫方法，描述对象可实现的行为）。

- 访问控制：

  C++ 提供了三个访问控制关键字：`public`、`private` 和 `protected`。

  `public` 成员对所有代码可见，构成类的外部接口。任何人都可以调用 `public` 的成员函数、读写 `public` 的成员变量。

  `private` 成员只有类自己的成员函数（以及友元）可以访问，外部代码不可见。

  `protected` 和 `private` 类似，但派生类也能访问。

- 成员函数定义：

  1. 类内定义：适合简单函数

     ```c++
     class Point
     {
     private:
         double x;
         double y;
     
     public:
         double get_x() const
         {
             return x;
         }
         double get_y() const
         {
             return y;
         }
     };
     ```

  2. 类内声明，类外定义：需要通过作用域解析运算符 `::` 声明函数属于哪个类。

     ```c++
     // point.hpp
     class Point
     {
     private:
         double x;
         double y;
     
     public:
         void set(double new_x, double new_y);
         double distance_to(const Point &other) const;
         void print() const;
     };
     
     // point.cpp
     #include <cmath>
     #include <iostream>
     
     #include "point.hpp"
     
     void Point::set(double new_x, double new_y)
     {
         x = new_x;
         y = new_y;
     }
     
     double Point::distance_to(const Point &other) const
     {
         double dx = x - other.x;
         double dy = y - other.y;
         return std::sqrt(dx * dx + dy * dy);
     }
     
     void Point::print() const
     {
         std::cout << "(" << x << ", " << y << ")";
     }
     ```

- `const` 对象只能调用 `const` 成员函数。

  编写成员函数时，如果该函数不需要修改对象，则需要给该函数加上 `const`。

### 构造函数

---

**默认构造 - 无参构造：**

构造函数用 `ClassName()` 声明。

```c++
class Point
{
private:
    double x_;
    double y_;

public:
    Point() : x_(0.0), y_(0.0)
    {
    }
};
```

如果一个构造函数都不写，编译器会生成一个默认构造函数。但是其对基本类型不做初始化，因此需要自己写默认构造函数。

如果构造了任意一个默认构造函数，编译器不再生成默认构造函数。此时仍然需要补充默认构造函数，或者用 `default` 生成。

```c++
class Point
{
private:
    double x_;
    double y_;

public:
    Point() = default; // 让编译器生成默认构造
    Point(double x, double y) : x_(x), y_(y)
    {
    }
};
```

---

**有参构造：**

有参构造接受参数初始化成员变量：

```c++
class Point
{
private:
    double x_;
    double y_;

public:
    Point(double x, double y) : x_(x), y_(y)
    {
    }
};

Point origin(0.0, 0.0);
```

当一个构造函数只接受一个参数时，传入参数会自动进行隐式类型转换，`explicit` 禁止这类转换。

单参数构造函数都应该用 `explicit` 修饰。

```c++
class PWMChannel
{
private:
    int channel_;

public:
    explicit PWMChannel(int ch) : channel_(ch) 
    {
    }
};

void set_active(PWMChannel ch);

// set_active(3);           // 编译错误！不能隐式转换
set_active(PWMChannel(3));
```

---

**成员初始化列表：**

构造函数的初始化列表写在参数列表后面的冒号之后，用逗号分隔，每个成员后面跟一个括号（或花括号）里的初始值。

```c++
class Sensor
{
private:
    int pin_;
    double threshold_;

public:
    Sensor(int pin, double threshold)
        : pin_(pin), threshold_(threshold)
    {
    }
};
```

对于 `const` 成员和引用成员，初始化列表是唯一合法的初始化方式。

成员的初始化顺序由它们在类定义中的声明顺序决定，跟初始化列表里的书写顺序无关。

---

**拷贝构造：**

拷贝构造函数从一个已存在的同类型对象创建新对象，固定为 `ClassName(const ClassName& other)`：

```c++
class Point
{
private:
    double x_;
    double y_;

public:
    Point(double x, double y) : x_(x), y_(y)
    {
    }

    // 拷贝构造函数
    Point(const Point &other) : x_(other.x_), y_(other.y_)
    {
    }
};

Point a(1.0, 2.0);
Point b = a;
Point c(a);  
```

拷贝构造函数在三种场景下会被调用：拷贝初始化、函数按值传参（形参通过拷贝构造创建）、函数按值返回（返回值通过拷贝构造复制）。

不编写拷贝构造函数时，编译器生成默认的拷贝构造函数，其行为是逐成员拷贝，即对每个成员分别调用其拷贝构造（对基本类型直接复制值）。

> 如果成员变量有指针，逐成员拷贝只会复制指针的值（地址），而不是指针指向的内容。此时两个对象的指针指向同一块内存，如果一个对象析构，则另一个变成悬空指针，这是浅拷贝问题。

如果类管理了资源（动态内存、文件句柄、网络连接等），就必须写拷贝构造函数。

---

**委托构造：**

允许一个构造函数在初始化列表里调用同一个类的另一个构造函数。

```c++
class Point
{
private:
    double x_;
    double y_;

public:
    // 主构造函数
    Point(double x, double y) : x_(x), y_(y) {

    }

    // 默认构造函数：委托给主构造函数
    Point() : Point(0.0, 0.0) {

    }
};
```

在初始化列表内调用另一个构造函数，先执行目标构造函数的初始化列表和函数体，然后控制权回到委托构造函数的函数体。

**初始化列表里一旦出现了委托，就不能再初始化任何成员。**

### 析构函数

析构函数用 `~ClassName()` 声明，没有参数和返回类型。一个类只有一个析构函数，不支持重载。

如果未定义析构函数，编译器默认生成析构函数，按照成员声明的逆序析构非静态成员。如果类管理了外部资源（动态内存，文件句柄，网络连接）必须手写析构函数。

栈对象在离开作用域时自动析构，堆对象只能用显式 `delete` 析构：

```c++
void process()
{
    FileWriter writer("log.txt", "w");
    writer.write("Processing started\n");
} // writer 在这里析构，文件自动关闭

void leaky()
{
    FileWriter *writer = new FileWriter("log.txt", "w");
    writer->write("Oops\n");
    // 忘了 delete —— 析构不调用，文件永远不会关闭
}
```

成员对象的析构发生在包含类的析构函数体执行完毕之后，顺序与构造严格相反。

### 静态成员

非静态成员绑定在对象上，每一个对象都有独立的非静态成员。**静态成员属于整个类，由所有对象共享。**

---

**静态成员变量：**

```c++
class Employee
{
private:
    int id_;
    std::string name_;
    static int next_id_; // 所有 Employee 共享的计数器
};
```

**静态成员变量必须在类外进行定义和初始化**；类内只能声明，但是不分配内存。如果没有在类外进行定义和初始化，会在链接阶段报错。

>C++17 后可以使用 `inline static` 在类内定义和初始化静态成员变量。

`static constexpr` 成员可以在类内直接初始化，无需类外定义（编译阶段确定值）。

---

**静态成员函数：**

静态成员函数没有 `this` 指针 (因为不需要知道调用它的是哪个对象)。

**静态函数无法访问非静态成员**。

调用静态成员函数推荐用 `ClassName::FunctionName()` 而不需要创建对象。

### 友元

友元用于外部函数去访问类的私有成员。

---

**友元函数：**

使用 `friend` 修饰普通函数声明，该函数为友元函数。

```c++
class Vector3D
{
private:
    float x, y, z;

public:
    Vector3D(float x, float y, float z) : x(x), y(y), z(z)
    {
    }
    
    friend float dot_product(const Vector3D &a, const Vector3D &b); // dot_product 为友元函数
};

float dot_product(const Vector3D &a, const Vector3D &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
```

友元函数不是成员函数，只是获得了访问私有成员变量权限的成员函数。

`friend` 声明可以在类的任何位置，一般集中放在类的开头或者末尾。

友元函数的一个应用是重载运算符：

```c++
class Point
{
private:
    int x, y;

public:
    Point(int x, int y) : x(x), y(y)
    {
    }

    // 友元重载 operator<<
    friend std::ostream &operator<<(std::ostream &os, const Point &p);
};

std::ostream &operator<<(std::ostream &os, const Point &p)
{
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

Point p(3, 4);
std::cout << p << std::endl; 
```

---

**友元类：**

友元类可以访问类的所有私有成员。

```c++
class Matrix
{
private:
    float data[3][3];

public:
    Matrix()
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                data[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    // Vector 是 Matrix 的友元类
    friend class Vector;
};

class Vector
{
private:
    float x, y, z;

public:
    Vector(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector transform(const Matrix &m)
    {
        // Vector 的成员函数可以直接访问 Matrix 的 private 成员
        float nx = m.data[0][0] * x + m.data[0][1] * y + m.data[0][2] * z;
        float ny = m.data[1][0] * x + m.data[1][1] * y + m.data[1][2] * z;
        float nz = m.data[2][0] * x + m.data[2][1] * y + m.data[2][2] * z;
        return Vector(nx, ny, nz);
    }
};
```

----

**友元成员函数：**

友元成员函数可以访问类的私有成员。

```c++
class Matrix
{
private:
    float data[3][3];

public:
    Matrix();
    // Vector::transform 是友元成员函数
    friend Vector Vector::transform(const Matrix &m);
};

class Vector
{
private:
    float x, y, z;

public:
    Vector(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector transform(const Matrix &m);
};
```

---

运算符重载，紧耦合的实现可以考虑使用友元。但是其他情况下使用友元需要多加考虑。

友元不能继承，不能传递(A是B的友元，B是C的友元，但是A不是C的友元)。

友元的声明不是函数前向声明。

### `this` 指针

`this` 的类型是 `ClassName* const`，即指向当前对象的常量指针。`this` 的指向不可改变。

`this` 指针可以用在参数名和成员变量名相同时，用于消除歧义。

```c++
class Point
{
    int x_;
    int y_;

public:
    Point(int x_, int y_) : x_(x_), y_(y_) {}

    void set_x(int x_)
    {
        this->x_ = x_;
    }
};
```

但是一般避免这样使用。

`*this` 是当前对象，所以成员函数返回 `*this` 的引用，可以使得调用者在一条语句里面连续调用多个方法：

```c++
class Point
{
    int x_;
    int y_;

public:
    Point() : x_(0), y_(0)
    {
    }

    Point &set_x(int x)
    {
        x_ = x;
        return *this;
    }

    Point &set_y(int y)
    {
        y_ = y;
        return *this;
    }

    Point &print()
    {
        std::cout << "(" << x_ << ", " << y_ << ")" << std::endl;
        return *this;
    }
};

Point p;
p.set_x(3).set_y(4).print();
```

## 6. 运算符重载

运算符重载可以增加代码的可读性，比如对向量重载加减法等等。

运算符重载的格式为 `返回类型 operator运算符()`。

运算符可以通过成员函数和非成员函数进行重载。如果作为成员函数重载，则运算符的左侧操作数必须是当前类的对象（比如 `Fraction(2, 3) + 1` 可以工作，`1` 通过构造函数进行隐式转换；但是 `1 + Fraction(2, 3)` 不行，因为 `int` 内没有 `operator+`）。作为非成员函数重载则没有限制。

---

**算术和比较运算符：**

赋值类的运算符（`=`，`+=`，`-=`，`[]`，`()`）必须是成员函数。

因此常用的实现模式是：将复合赋值运算符使用成员函数重载，然后基于此实现二元运算符的非成员函数重载。

```c++
class Fraction
{
private:
    int numerator_;   // 分子
    int denominator_; // 分母

public:
    Fraction(int num = 0, int den = 1)
        : numerator_(num), denominator_(den)
    {
        if (denominator_ == 0)
        {
            denominator_ = 1;
        }
        normalize();
    }

    Fraction &operator+=(const Fraction &rhs)
    {
        // a/b + c/d = (a*d + c*b) / (b*d)
        numerator_ = numerator_ * rhs.denominator_ + rhs.numerator_ * denominator_;
        denominator_ *= rhs.denominator_;
        normalize();
        return *this;
    }

    int num() const { return numerator_; }
    int den() const { return denominator_; }

private:
    void normalize()
    {
        int g = gcd(numerator_, denominator_);
        numerator_ /= g;
        denominator_ /= g;
        if (denominator_ < 0)
        {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
    }

    static int gcd(int a, int b)
    {
        a = (a < 0) ? -a : a;
        b = (b < 0) ? -b : b;
        while (b != 0)
        {
            int t = b;
            b = a % b;
            a = t;
        }
        return (a == 0) ? 1 : a;
    }
};

// 非成员函数重载 +
Fraction operator+(Fraction lhs, const Fraction& rhs)
{
    lhs += rhs;  // 复用 operator+=
    return lhs;  
}
```

> - 复合赋值由于是就地修改，因此需要返回 `*this` 的引用而不是按值返回。
> - 二元运算符必须按值返回而不是引用，因为其计算结果是一个新的值。

比较运算符中，`operator!=` 基于 `operator==` 实现；`operator>`，`operator>=`，`operator<=` 基于 `operator<` 实现。

```c++
bool operator==(const Fraction &lhs, const Fraction &rhs)
{
    return lhs.num() == rhs.num() && lhs.den() == rhs.den();
}

bool operator!=(const Fraction &lhs, const Fraction &rhs)
{
    return !(lhs == rhs);
}

bool operator<(const Fraction &lhs, const Fraction &rhs)
{
    return lhs.num() * rhs.den() < rhs.num() * lhs.den();
}

bool operator>(const Fraction &lhs, const Fraction &rhs)
{
    return rhs < lhs;
}

bool operator<=(const Fraction &lhs, const Fraction &rhs)
{
    return !(rhs < lhs);
}

bool operator>=(const Fraction &lhs, const Fraction &rhs)
{
    return !(lhs < rhs);
}
```

---

**流与下标运算符：**

`<<` 的重载用非成员函数实现（因为 `std::cout` 不是这个类），一般声明为友元。

 ```c++
 friend std::ostream& operator<<(std::ostream& os, const Fraction& f)
 {
     if (f.denominator == 1) {
         os << f.numerator;      
     }
     else {
         os << f.numerator << "/" << f.denominator;
     }
     return os;
 }
 ```

`>>` 的重载用非成员函数实现，但是第二个参数不是 `const` 引用，因为要写入数据。

```c++
friend std::istream& operator>>(std::istream& is, Fraction& f)
{
    int num, denom;
    char slash;

    is >> num >> slash >> denom;

    if (is && slash == '/' && denom != 0) {
        f.numerator = num;
        f.denominator = denom;
        f.reduce();
    }
    else {
        is.setstate(std::ios::failbit);
    }

    return is;
}
```

`[]` 的重载用成员函数实现，需要提供 `const` 版本（返回只读引用）和非 `const` 版本（返回可修改引用）。

> `[]` 没有边界检查，需要边界检查可以考虑使用 `.at()`。

---

**函数调用运算符：**

```c++
class Multiplier
{
private:
    int factor_;

public:
    explicit Multiplier(int factor) : factor_(factor)
    {
    }

    int operator()(int x) const
    {
        return x * factor_;
    }
};

Multiplier triple(3);
int result = triple(10);
```

`operator()` 不能作为非成员重载，`operator()` 的参数类型，参数个数，返回类型可以自由选择。在类内实现 `operator()` 重载后，可以让对象作为一个函数使用（称为函数对象）。

---

**类型转换运算符：**

类型转换运算符 `operator 目标类型()` 允许一个类的对象被隐式或显式地转换为另一种类型，没有返回类型声明。

```c++
class NullableInt
{
private:
    int value_;
    bool has_value_;

public:
    NullableInt(int v) : value_(v), has_value_(true)
    {
    }

    NullableInt() : value_(0), has_value_(false)
    {
    }

    operator bool() const
    {
        return has_value_;
    }

    operator int() const
    {
        return value_;
    }
};

NullableInt a(42);

if (a)  // 隐式转换为 bool
{
    int x = a; // 隐式转换为 int
}
```

一般需要用 `explicit` 修饰符避免隐式转换。

## 7. 继承和多态

### 单继承

```c++
class Person
{
private:
    std::string name_;
    int age_;

public:
    Person(const std::string &name, int age)
        : name_(name), age_(age)
    {
    }

    const std::string &name() const
    {
        return name_;
    }

    int age() const
    {
        return age_;
    }
};

class Student : public Person
{
private:
    std::string school_;

public:
    Student(const std::string &name, int age, const std::string &school)
        : Person(name, age), school_(school)
    {
    }

    const std::string &school() const
    {
        return school_;
    }
};
```

继承的语法是 `class 派生类: 继承方式 基类`。

- 继承方式：

  - `public` 继承：基类的 `public` 成员仍然 `public`，基类的 `protected` 成员仍然 `protected`，基类的 `private` 成员不可访问。
  - `protected` 继承：基类的 `public` 成员是 `protected`，基类的 `protected` 成员仍然 `protected`，基类的 `private` 成员不可访问。
  - `private` 继承：基类的 `public` 成员是 `private`，基类的 `protected` 成员是 `private`，基类的 `private` 成员不可访问。

  一般使用 `public` 继承。

继承的构造和析构顺序是：构造时由基类到派生类，析构由派生类到基类。

- 隐藏

  基类中定义的函数如果在派生类中再次定义，则基类中所有的同名函数都会被隐藏。

  ```c++
  #include <iostream>
  using namespace std;
  
  class A {
  public:
      void f() 
      {
          cout << "A::f()" << endl;
      }
  };
  
  class B : public A {
  public:
      void f() 
      {   
          cout << "B::f()" << endl;
      }
  };
  
  int main() {
      B b;
      b.f();   // "B::f()"
      return 0;
  }
  ```

  如果想调用基类被隐藏的函数，则需要使用 `b.A::f()` 显式指定作用域。

  如果想保留基类被隐藏函数的重载集合，可以在派生类内加一句 `using A::f;`。

- 对象切片

  如果将派生类对象赋值给基类对象，则派生类特有的成员和行为将会被切掉。

  一般使用引用或者指针进行传递可以避免对象切片。

### 虚函数 - 多态

```c++
#include <iostream>
using namespace std;

class A
{
public:
    void f() const
    {
        cout << "A::f()" << endl;
    }
};

class B : public A
{
public:
    void f() const
    {
        cout << "B::f()" << endl;
    }
};

class C : public A
{
public:
    void f() const
    {
        cout << "C::f()" << endl;
    }
};

int main()
{
    A *a[3];
    a[0] = new A();
    a[1] = new B();
    a[2] = new C();

    for (int i = 0; i < 3; i++)
    {
        a[i]->f();
    }

    for (int i = 0; i < 3; i++)
    {
        delete a[i];
    }

    return 0;
}
```

三次调用 `f()` 是均调用基类的 `f()`，编译器在编译 `a[i]->f();` 时，只看到 `a[i]` 的静态类型为 `A*`，于是绑定函数为 `A::f()`，并不关心运行时的实际指向，称为静态绑定。

如果需要统一接口，不同行为（多态）时，需要进行动态绑定。

```c++
#include <iostream>
using namespace std;

class A
{
public:
    virtual void f() const
    {
        cout << "A::f()" << endl;
    }
};

class B : public A
{
public:
    void f() const override
    {
        cout << "B::f()" << endl;
    }
};

class C : public A
{
public:
    void f() const override
    {
        cout << "B::f()" << endl;
    }
};

int main()
{
    A *a[3];
    a[0] = new A();
    a[1] = new B();
    a[2] = new C();

    for (int i = 0; i < 3; i++)
    {
        a[i]->f();
    }

    for (int i = 0; i < 3; i++)
    {
        delete a[i];
    }

    return 0;
}
```

在基类函数前用 `virtual` 修饰，则派生类中签名匹配的函数也是虚函数。此时每个对象根据自己实际的类型进行函数调用，即为动态绑定。

`override` 会强制编译器检查是否正确重写基类的虚函数。能捕获的错误包括但不限于：基类中根本不存在这个名字的虚函数、函数签名不匹配（`const`、引用限定符等差异）、基类函数不是 `virtual` 的。

如果需要通过基类指针 `delete` 派生类对象，基类的析构函数必须是 `virtual` 的，不然会导致静态绑定后，基类相关成员析构但是派生类相关成员不进行析构。

### 抽象类 - 接口

类中至少声明一个纯虚函数以成为抽象类：

```c++
class Shape
{
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;      // 纯虚函数
    virtual const char *name() const = 0; // 纯虚函数
};
```

纯虚函数在虚函数的基础上，声明末尾加上了 `=0`；纯虚函数在基类中没有实现，派生类必须提供自己的版本。

抽象类无法直接创建对象，但是可以指针和引用。

如果派生类没有实现某个纯虚函数，则派生类也会成为抽象类。

抽象类设计为能进行的行为，派生类设计为行为的具体实现。

---

**接口隔离：**

不要去实现一个大接口，而是将大接口拆分成小而聚焦的接口。

### 多继承

多继承指派生类由多个基类派生而来。

```c++
#include <iostream>
using namespace std;

class Printer
{
public:
    void print(const string &s)
    {
        cout << "Printing: " << s << endl;
    }
};

class Scanner
{
public:
    void scan()
    {
        cout << "Scanning..." << endl;
    }
};

class AllInOne : public Printer, public Scanner
{
};
```

多继承的构造顺序是基类按照继承列表声明顺序构造，析构恰好相反。

如果基类有同名成员，会造成歧义。

多继承最安全的用法是接口继承：所有基类是纯虚接口，不包含成员和具体实现。

---

**菱形继承问题：**

一个基类被两个中间类继承，最终类同时继承这两个中间类，形成菱形。不做特殊处理时，最终对象中会包含两份共同基类子对象。

```c++
class Device
{
public:
    int id;
    Device() : id(0)
    {
    }
};

class InputDevice : public Device
{
};
class OutputDevice : public Device
{
};

class TouchScreen : public InputDevice, public OutputDevice
{
};
```

`Device` 被调用两次，导致 `id` 被独立拷贝两份。

解决该问题的方案是虚继承，在中间层类继承共同基类时加上 `virtual` 即可。

```c++
class InputDevice : virtual public Device
{
};

class OutputDevice : virtual public Device
{
};

class TouchScreen : public InputDevice, public OutputDevice
{
public:
    TouchScreen() : Device(), InputDevice(), OutputDevice() 
    {
    }
};
```

此时虚基类的构造由最底层派生类负责。中间层构造函数对虚基类的初始化列表会被静默忽略。

虚继承必须出现在所有直接继承共同基类的中间类上。

多继承，虚继承是十分复杂的，很多时候考虑使用组合去替代继承：如果一个类需要同时具备多种能力但不要求通过基类指针统一操作，直接持有成员对象往往比继承更清晰。

如果需要运行时多态，可以使用接口委托：定义一个接口类，内部通过指针委托给具体实现。

## 8. 模板

### 函数模板

例如需要编写不同类型的 `max` 函数，比如 `max_int`，`max_double`，`max_string`，需要写三个版本的函数，仅仅是传入参数不同。对于这种逻辑相同，类型不同的重复代码，可以使用函数模板实现泛型。

```c++
template <typename T>
T max_value(T a, T b)
{
    return (a > b) ? a : b;
}
```

`template <typename T>` 告诉编译器这是一个模板，`T` 是一个类型参数。在紧跟其后的函数定义中，所有 `T` 在实例化时会被替换为实际类型，例如 `max_value(3, 5)` 中的 `T` 自动推导为 `int`，则生成 `int max_value(int, int)` 的函数版本。

可以使用多个模板类型参数：

```c++
template <typename Dest, typename Source>
Dest cast_to(Source value)
{
    return static_cast<Dest>(value);
}
```

---

**模板实例化：**

编译器一般根据调用的参数自动推导类型。

可以强制指定模板类型：

```c++
int result = max_value<double>(3, 5.0);
```

---

**模板特化：**

模板特化允许对某个特定类型提供专门实现：

```c++
template <typename T>
T max_value(T a, T b)
{
    return (a > b) ? a : b;
}

// const char* 的特化版本
template <>
const char* max_value<const char*>(const char* a, const char* b)
{
    return (std::strcmp(a, b) > 0) ? a : b;
}
```

`template <>` 表示这是一个完全特化，所有模板参数都已确定。

### 类模板

类模板的定义从 `template <typename T>` 开始，后面紧跟类的定义。所有出现 `T` 的地方在实例化时都会被替换为实际类型——包括成员变量、成员函数参数、返回类型，甚至友元声明。

```c++
template <typename T>
class Stack
{
public:
    void push(const T &value);
    void pop();
    T &top();
    const T &top() const;
    bool empty() const;
    std::size_t size() const;

private:
    std::vector<T> data_;
};
```

使用类模板时，必须提供具体的模板参数。

类模板的成员函数可以在类体内直接定义，也可以在类体外定义。类体内定义和普通定义方式没有差别，但是在类体外定义的成员函数，都必须带上完整的模板头部。

```c++
template <typename T>
void Stack<T>::push(const T &value)
{
    data_.push_back(value);
}
```

---

**模板参数：**

类型参数是最常见的模板参数。

非类型参数是编译期的常量，比如指定容器容量：

```c++
template <typename T, std::size_t kCapacity>
class RingBuffer
{
public:
    void push(const T &value)
    {
        buffer_[write_index_] = value;
        write_index_ = (write_index_ + 1) % kCapacity;
    }

private:
    std::array<T, kCapacity> buffer_;
    std::size_t write_index_ = 0;
};

RingBuffer<int, 16> buffer;
```

非类型参数只能是整型、枚举、指针、引用。

模板参数也支持默认值，从右向左连续提供。

### 模板特化

---

**全特化：所有模板参数确定**

```c++
template <typename T>
class Stack
{
public:
    void push(const T &value)
    {
        data_.push_back(value);
    }

    void pop()
    {
        data_.pop_back();
    }

    T top() const
    {
        return data_.back();
    }

    bool empty() const
    {
        return data_.empty();
    }

private:
    std::vector<T> data_;
};

// bool 全特化版本
template <>
class Stack<bool>
{
public:
    void push(bool value)
    {
        bits_.push_back(value);
    }

    void pop()
    {
        bits_.pop_back();
    }

    bool top() const
    {
        return bits_[bits_.size() - 1];
    }
    
    bool empty() const
    {
        return bits_.empty();
    }

private:
    std::vector<bool> bits_; // 空间优化的 bit 容器
};
```

类的特化版本和通用版本之间没有任何代码复用关系——特化类是一个完全独立的类，它可以有不同的数据成员、不同的成员函数、甚至不同的接口设计。

函数模板的显式特化不参与重载决议。对于函数模板，优先使用重载而不是显式特化。

---

**偏特化：部分模板参数确定**

```c++
template <typename T>
struct Printer
{
    static void print(const T &value)
    {
        std::cout << value;
    }
};

// 偏特化：匹配所有指针类型 T*
template <typename T>
struct Printer<T *>
{
    static void print(T *ptr)
    {
        if (ptr)
        {
            std::cout << "*";
            Printer<T>::print(*ptr); 
        }
        else
        {
            std::cout << "(null)";
        }
    }
};
```

编译器会在所有可匹配的版本中选择最特殊的那一个。

