# C++ 现代特性

## 1. 移动语义和右值引用

---

**C++ 的值类别：**

C++ 的值类别包括 `lvalue`，`xvalue`，`prvalue`，这三个类别可以组成更宽泛的类别。

> - `lvalue` 是有身份（有名字，可取地址），不可移动（表达式不是临时的，不可被安全的偷走资源）；
> - `xvalue` 是有身份且可移动的值；
> - `prvalue` 是没有身份且可移动的值。

使用 `decltype` 类别可以对值类型进行判断：

- `decltype(x)`（不带括号的标识符）得到 `x` 的声明类型；
- `decltype((x))`（带括号的表达式）按值类别：`lvalue` 得 `T&`，`xvalue` 得 `T&&`，`prvalue` 得 `T` 本身。随后可以通过 `std::is_lvalue_reference_v` / `std::is_rvalue_reference_v` 进行判断。

---

**右值引用 `T&&`：**

右值引用只能绑定到 `xvalue` 和 `prvalue` 上：

```c++
int&& r1 = 42;           // prvalue
int&& r2 = x + 1;        // prvalue
int&& r3 = std::move(x); // xvalue
```

这条规则约束写死类型的右值引用，如 `int&&`、`std::string&&`。

> **`const` 左值引用和右值引用：**
>
> - `const` 左值引用可以绑定到任何东西上，用 `const T&` 接收右值时，没法偷走它的资源；
> - 右值引用接收右值，用 `T&&` 接收一个右值时，有修改权限，所以可以安全地把资源转移走。

---

**`std::move` 函数：**

`std::move` 将参数转换为右值引用（`static_cast<T&&>`）。

```c++
std::string a = "Hello";
std::string b = std::move(a);		// a 转换为 std:string&&，允许 b 通过移动构造转移 a 的资源
									// b 通过移动构造转移 a 的字符缓冲区，a 此时仍然有效，但是指定的字符缓冲区变空
```
```c++
std::vector<std::string> names;

std::string name = "Alice";
names.push_back(std::move(name));  	// 将 name 的数据移动到 vector 中，name 的数据被搬空
names.push_back("Bob");   			// 先从 const char* 构造临时对象，再移动进 vector
```

---

**生命周期：**

C++ 中临时对象的生命周期通常在包含它的完整表达式结束时直接终止，但是右值引用在绑定到临时对象时，会延长该对象的生命周期，直到引用的作用域结束。

```c++
int&& rr = 100;            	// 右值引用延长了 100 的生命周期
std::cout << rr << "\n";   
rr = 200;					// 100 可以“被修改”，编译器将 100 放在一块存储空间里，rr 指向的存储空间可以被修改，实际上是延长了这段存储空间的生命周期
```

生命周期的延长只对直接绑定生效，不跨函数边界。

---

**移动构造：**

拷贝构造的问题在于：不需要源对象时，拷贝构造函数仍然复制每一个字节，然后源对象析构时又释放掉原来的内存。移动构造的思想是：不修改数据，只需要转移资源的所有权。相当于从源对象偷来指针，并将源对象的指针置空避免析构时释放这块内存。

```c++
class Buffer {
    char* data_;
    std::size_t size_;
    std::size_t capacity_;

public:
    // 构造和析构函数

    // 移动构造函数
    Buffer(Buffer&& other) noexcept
        : data_(other.data_)
        , size_(other.size_)
        , capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
};
```

移动构造函数需要用右值触发，此时只需要拷贝整型/指针的赋值，成本极低。

**移动赋值：**

移动赋值时目标对象可能已经持有资源，因此必须将目标对象的资源释放后接管新的资源。

```c++
class Buffer {
    // 成员函数/成员变量

    // 移动赋值运算符
    Buffer& operator=(Buffer&& other) noexcept
    {
        if (this != &other) {
            // 第一步：释放当前持有的资源
            delete[] data_;

            // 第二步：接管 other 的资源
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;

            // 第三步：置空 other
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }
};
```

移动构造和移动赋值可以标记 `noexcept`，主要是关于 `std::vector` 的扩容行为。当 `vector` 需要增长容量时，它必须把现有元素转移到新的内存块。如果元素的移动构造函数是 `noexcept` 的，`vector` 会放心地使用移动；如果移动构造函数可能抛异常，`vector` 会退而使用拷贝构造——因为在移动过程中如果抛异常，已经移动了一半的状态很难恢复，但拷贝过程中抛异常，原来的数据还完好无损。

一般的，对于管理资源的类。需要自定义析构函数，拷贝构造函数，拷贝赋值运算符，移动构造函数，移动赋值运算符。

---

**RVO/NRVO：**

在 C 中，按值返回一个结构体，意味着函数里构造一份，再把这份拷贝给调用者；这笔开销在性能敏感的代码中不能接受。C++ 提供返回值优化 RVO 和命名返回值优化 NRVO，其思路是：最终的对象在调用者的栈上，因此不必在函数内部进行构造然后拷贝/移动，而是直接在调用者的空间内构造。

```c++
Point make_point_rvo(double x, double y)
{
    return Point(x, y);   // 返回一个临时对象，RVO
}

Point make_point_nrvo(double x, double y)
{
    Point p(x, y);        
    return p;             // 返回命名变量，NRVO
}
```

NRVO 会在多个返回分支返回不同的命名对象时失效（退化为移动），编译器要做 NRVO，需要在调用者的空间里预先分配好内存，然后让函数内部的命名变量直接构造在这个空间里。但如果有两个不同的命名变量可能被返回，编译器就没法把两个变量都放在同一块空间里，它们各有各的地址。返回函数参数同样会使得 NRVO 失效。

使用以下规则可以最大化 RVO/NRVO 收益：

> 1. 按值返回，不用输出参数；
> 2. 不要写 `return std::move(local);`
> 3. 保持返回路径简单。

---

**完美转发：**

```c++
// 按 const 引用接收，进行拷贝构造
// 能接受左值，但传入右值时也会拷贝，因为丢失了右值信息（无法拿走资源）
std::string make_string(const std::string& s)
{
    return std::string(s); 
}

// 按右值引用接收，进行移动构造
// 不能接受左值
std::string make_string(std::string&& s)
{
    return std::string(std::move(s));  // 总是移动构造
}
```

为了区分 `const` 引用和右值引用，必须提供两个重载。为此可以使用转发引用用于推导传入的是左值还是右值：

```c++
template<typename T>
void identify(T&& arg)
{
   // arg 通过调用时传入的实参推导
}

identify(name);              	// 传左值，T = std::string&，T&& = std::string&
identify(std::string("Bob")); 	// 传右值，T = std::string，T&& = std::string&&
```

转发引用的类型必须通过模板参数推导，声明形式必须是 `T&&`，不能用 `const` 或者其他修饰。模板参数推导中的 `T&&` 会根据传入的实参自动调整：传左值进来，`T` 推导为左值引用类型，`T&&` 通过**引用折叠**变成左值引用；传右值进来，`T` 推导为非引用类型，`T&&` 就是右值引用。

转发引用通过引用折叠进行工作：`T& &` 折叠为 `T&`，`T& &&` 折叠为 `T&`，`T&& &` 折叠为 `T&`，`T&& &&` 折叠为 `T&&`。（只有左值会发生引用折叠）

`std::forward` 的作用是：传入的是右值时，把参数转成右值引用（返回右值引用）；传入的是左值时，保持左值引用不变（返回左值引用）。

转发引用-引用折叠-`std::forward` -> 完美转发：比如 `std::make_unique` ，接收任意参数，然后原封不动地转发给 `unique_ptr`/`shared_ptr` 所管理对象的构造函数。

```c++
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
```

使用时只对转发引用使用 `std::forward`，并且只能使用一次 `std::forward`。

---

**使用移动语义：**

对于管理了动态资源的类（持有 `new` 出来的内存、`fopen` 打开的文件、或者类似的资源句柄），应该实现完整的规则五：自定义析构函数、拷贝构造、移动构造、拷贝赋值、移动赋值。移动构造和移动赋值中要把源对象的资源指针置空，确保源对象析构时不会释放已转移的资源。只要移动操作保证不抛出异常，就应该标记 `noexcept`（绝大多数情况下移动操作只是指针复制，不会抛出异常）。

对于只持有基本类型和标准库容器的类，通常可以用 `= default` 让编译器生成移动操作。`std::string`、`std::vector`、`std::map` 这些标准库组件都有高效的移动语义，编译器自动生成的移动构造函数会按照成员声明顺序逐个调用成员的移动构造函数（对类成员）或直接复制（对标量成员）。

对于封装了独占资源的类（文件句柄、网络连接、锁），应该**禁用拷贝、启用移动**。（嵌入式外设句柄应该启用移动语义）

## 2. 智能指针和 RAII

RAII（Resource Acquisition Is Initialization）是 C++ 最核心的资源管理思想：**资源的获取放在构造函数里，资源的释放在析构函数里**。RAII 通过栈展开实现，不管控制流如何离开作用域，所有已构造的局部对象都会被依次销毁。

> 析构函数保证只适用于正常控制流退出。如果程序调用 `std::exit()`、`std::abort()` 或 `_exit()`，或者被信号杀死，栈展开不会发生，局部对象的析构函数也不会被调用。这也是为什么应该优先使用异常而非 `std::exit()` 的原因之一。
>
> 析构函数应保证不抛异常。在异常传播（栈展开）期间若析构函数抛出新异常，程序会调用 `std::terminate()`。C++11 起，用户声明的析构函数默认为 `noexcept(true)`（即使没有显式指定），抛出异常即终止。因此析构函数中应捕获并处理所有异常，或将可能失败的操作移出析构函数，提供显式接口处理错误。

RAII 包装器遵循以下设计模式：构造函数负责获取资源（如果获取失败则抛异常或进入无效状态），析构函数负责释放资源（必须 `noexcept`），禁止拷贝（防止双重释放），允许移动（支持所有权转移）。

### `unique_ptr` - 独占所有权的零开销智能指针

同一时刻，只有一个 `unique_ptr` 拥有对象的所有权，这意味着不允许拷贝构造和拷贝赋值，只允许移动。

```c++
#include <memory>

auto p1 = std::make_unique<int>(42);				// 创建 p1 为 unique_ptr 指针

// std::unique_ptr<int> p3 = p1;					// 编译错误，禁止拷贝
std::unique_ptr<int> p3 = std::move(p1);  			// 移动语义，将 p1 内容移动到 p3
```

创建 `unique_ptr` 指针通常需要使用 `make_unique` 函数，其代码简洁，更加安全，但是不支持自定义删除器。

`unique_ptr` 可以放在容器中，因为 `unique_ptr` 的移动操作均标记为 `noexcept`，因此容器只会使用移动。

`unique_ptr` 有一个针对数组的偏特化版本 `unique_ptr<T[]>`，它在析构时会调用 `delete[]` 而不是 `delete`，但是一般不用这个版本。

`unique_ptr` 的第二个模板参数是删除器类型，默认是 `std::default_delete<T>`，内部就是简单的 `delete ptr`。可以替换为任何可调用对象——函数指针、`lambda`、函数对象（ `void operator()(T*)` 的签名）。

```c++
auto fileDeleter = [](FILE* f) { 
    if (f) fclose(f); 
};

std::unique_ptr<FILE, decltype(fileDeleter)> filePtr(
    fopen("test.txt", "r"), fileDeleter
);
```

`unique_ptr` 是零开销的，但是前提是删除器是空类（无数据成员），此时编译器进行空基类优化（EBO），大小优化为 0，仅保存裸指针。

---

**工厂模式：**

工厂函数返回 `unique_ptr` 是一种非常常见的模式。它不仅安全，而且表达了清晰的所有权语义：工厂创建对象，调用者独占拥有。

```c++
#include <memory>
#include <iostream>

// 基类
class Shape {
public:
    virtual void draw() const = 0;
    virtual ~Shape() = default;
};

// 派生类
class Circle : public Shape {
public:
    void draw() const override { std::cout << "Drawing Circle\n"; }
};

class Square : public Shape {
public:
    void draw() const override { std::cout << "Drawing Square\n"; }
};

// 工厂函数：返回 unique_ptr<Shape>
enum class ShapeType { Circle, Square };

std::unique_ptr<Shape> createShape(ShapeType type) {
    switch (type) {
        case ShapeType::Circle:
            return std::make_unique<Circle>();
        case ShapeType::Square:
            return std::make_unique<Square>();
        default:
            return nullptr;
    }
}

// 使用
int main() {
    auto shape = createShape(ShapeType::Circle);
    shape->draw();  // Drawing Circle
    // 自动释放内存
}
```

---

**`unique_ptr` 的所有权管理：**

- `get()` 返回内部裸指针但不转移所有权。
- `release()` 放弃所有权并返回裸指针。
- `reset()` 替换当前管理的对象。

### `shared_ptr` - 共享所有权的智能指针

`shared_ptr` 表征一个对象确实需要被多个模块共同持有、共同管理。其核心思想是引用计数：每多一个 `shared_ptr` 指向对象，计数就加一；每少一个，计数就减一；当计数归零时，对象被自动销毁。

多个 `shared_ptr` 可以指向同一个对象，它们共同决定对象的生命周期。只有当最后一个 `shared_ptr` 被销毁时，对象才会被 `delete`。

每一个 `shared_ptr` 的拷贝和析构都需要更新引用计数，而引用计数必须是线程安全的（原子操作）。此外，`shared_ptr` 内部还需要维护一个控制块来存储引用计数和其他元信息。这些开销在频繁创建和销毁 `shared_ptr` 的场景下会变得非常明显。

>一个 `shared_ptr` 包含两个指针：一个指向被管理的对象，另一个指向控制块。
>
>控制块是一个在堆上分配的数据结构，包含强引用计数（`shared_ptr` 的数量）、弱引用计数（`weak_ptr` 的数量）、自定义删除器（如果有的话）、自定义分配器（如果有的话）。
>
>当用 `std::make_shared` 创建 `shared_ptr` 时，对象和控制块会被放在同一个内存块中（一次分配）；而用 `std::shared_ptr<T>(new T)` 创建时，对象和控制块是两次独立的分配。
>
>`std::make_shared` 可以得到更少的堆分配次数，更好的缓存局部性，更少的内存碎片。

`shared_ptr` 的引用计数使用原子操作来保证线程安全。这意味着在多线程环境下可以安全地拷贝和销毁 `shared_ptr` 本身（引用计数的增减是原子的），但**被管理对象的访问并不受保护**——如果有多个线程同时读写对象本身，仍然需要自行加锁。

`shared_ptr` 不能滥用，不然会导致各种问题。只有在确定这个对象需要被多个独立的模块共同拥有时适合使用。

> 在嵌入式系统中优先使用 `unique_ptr` 或者直接使用 RAII 封装类。如果确实需要共享语义，考虑侵入式引用计数——把引用计数放在对象内部，避免额外的堆分配。在单线程环境下，侵入式方案的引用计数可以用普通的 `uint32_t`，不需要原子操作，开销极低。

### `weak_ptr` 

当两个对象互相持有对方的 `shared_ptr` 时，它们的引用计数永远不会归零。而 `std::weak_ptr`  是一种不参与引用计数的观察者指针——可以通过它查看对象，临时获取一个 `shared_ptr` 来访问。

`weak_ptr` 有三个 API：

- `lock()` 尝试获取一个指向对象的 `shared_ptr`。如果对象仍然存在（强引用计数 > 0），返回一个有效的 `shared_ptr`；如果对象已经被销毁（强引用计数 = 0），返回一个空的 `shared_ptr`（即 `nullptr`）。

  `lock()` 是线程安全的——在多线程环境下，多个线程可以同时调用 `lock()`，标准保证返回的 `shared_ptr` 要么指向一个有效对象，要么为空，不会出现"获取到指针但对象已被删除"的悬垂情况。

- `expired()` 返回一个 bool 值，表示对象是否已经被销毁（即强引用计数是否为 0）。不过在实际使用中，通常推荐直接用 `lock()` 而不是先检查 `expired()` 再 `lock()`——因为在多线程环境下，`expired()` 返回 `false` 之后到调用 `lock()` 之间，对象可能已经被另一个线程销毁了，这会导致竞态条件。

- `use_count()` 返回当前指向对象的 `shared_ptr` 数量（即强引用计数）。

`weak_ptr` 可以用在观察者模式中：主题用 `weak_ptr` 引用观察者（不延长观察者的生命周期），观察者可以选择用 `shared_ptr` 或 `weak_ptr` 引用主题。

### 自定义删除器/侵入式引用计数

智能指针一般管理 `new` 得到的对象，析构时调用 `delete`。但是可能需要管理的资源是 `fopen()` 返回的 `FILE*`（要用 `fclose` 关闭），可能是 `malloc()` 分配的内存（要用 `free` 释放），可能是 POSIX 的文件描述符 `int`（要用 `close` 关闭），可能是 SDL 的窗口、OpenGL 的纹理、CUDA 的 stream——每种资源都有自己的释放函数。

---

**自定义删除器：**

自定义删除器是一个可调用对象，在智能指针析构时被调用，负责释放资源。可以是函数指针、`lambda` 表达式、或者函数对象。

1. 函数指针：

   传入一个函数的地址，智能指针在析构时调用它。但函数指针有一个缺点：它会增加 `unique_ptr` 的大小，因为 `unique_ptr` 需要额外存储这个函数指针。

   ```c++
   #include <cstdio>
   #include <memory>
   #include <iostream>
   
   // 用函数指针管理 FILE*
   void close_file(FILE* f) noexcept {
       if (f) {
           std::cout << "fclose called\n";
           std::fclose(f);
       }
   }
   
   void file_example() {
       // unique_ptr<FILE, 函数指针类型>
       std::unique_ptr<FILE, void(*)(FILE*)> fp(std::fopen("/tmp/test.txt", "w"), close_file);
   
       if (fp) {
           std::fprintf(fp.get(), "hello from unique_ptr with custom deleter\n");
       }
   
       // 离开作用域时自动调用 close_file(fp.get())
   }
   ```

2. `lambda` 表达式：

   最常用的删除器形式。无捕获的 `lambda` 可以转换为函数指针，因此和函数指针的内存开销相同。但有捕获的 `lambda` 会成为有状态删除器，增加 `unique_ptr` 的大小。

   ```C++
   auto file_closer = [](FILE* f) noexcept {
       if (f) std::fclose(f);
   };
   using LambdaFilePtr = std::unique_ptr<FILE, decltype(file_closer)>;
   ```

3. 函数对象

   函数对象是无状态删除器的最佳选择——既没有函数指针的存储开销，又比 `lambda` 更容易复用和命名。
   
   ```C++
   struct FreeDeleter {
       void operator()(void* p) noexcept {
           std::free(p);
       }
   };
   
   // 为 malloc 返回的内存创建类型安全的智能指针
   template <typename T>
   using MallocPtr = std::unique_ptr<T, FreeDeleter>;
   ```

在 `unique_ptr` 中，删除器类型是 `unique_ptr` 类型的一部分。但是在 `shared_ptr` 中，删除器类型不是 `shared_ptr` 的一部分，它被存储在控制块中。因此可以用同一个 `shared_ptr<T>` 类型持有不同删除器的对象：

```c++
std::shared_ptr<void> make_resource(const std::string &type)
{
    if (type == "file")
    {
        return std::shared_ptr<void>(
            std::fopen("/tmp/test.txt", "w"),
            [](void *p) noexcept
            {
                if (p)
                    std::fclose(static_cast<FILE *>(p));
            });
    }
    else if (type == "malloc")
    {
        return std::shared_ptr<void>(
            std::malloc(1024),
            [](void *p) noexcept
            {
                std::free(p);
            });
    }
    return nullptr;
}
```

---

**侵入式引用计数：**

侵入式引用计数将引用计数嵌入到对象内部，而非外部分配控制块。

侵入式引用计数的实现首先需要定义一个基类，封装引用计数及对应增减接口：

```c++
class RefCounted
{
public:
    // 增加引用计数
    void add_ref() noexcept
    {
        ++ref_count_;
    }
    
    // 减少引用计数
    void release() noexcept
    {
        if (--ref_count_ == 0)
        {
            delete this;
        }
    }

protected:
    RefCounted() = default;
    virtual ~RefCounted() = default;

private:
    uint32_t ref_count_{1}; // 创建时默认持有一次
};
```

所有需要被共享管理的对象，只需继承 `RefCounted` 即可获得引用计数能力。

然后需要智能指针自动管理 `add_ref` 和 `release` 的调用，即为 `intrusive_ptr`：

```c++
template<typename T>
class IntrusivePtr {
private:
    T* ptr_{nullptr};

    void incref() {
        if (ptr_) {
            ptr_->add_ref();
        }
    }

    void decref() {
        if (ptr_) {
            ptr_->release();
        }
    }

public:
    IntrusivePtr() = default;

    // 从裸指针构造，增加引用
    explicit IntrusivePtr(T* p) noexcept : ptr_(p) {
        incref();
    }

    // 拷贝构造，增加引用
    IntrusivePtr(const IntrusivePtr& other) noexcept : ptr_(other.ptr_) {
        incref();
    }

    // 移动构造，无需增加引用
    IntrusivePtr(IntrusivePtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    // 拷贝赋值
    IntrusivePtr& operator=(const IntrusivePtr& other) noexcept {
        if (this != &other) {
            reset();          // 释放当前对象
            ptr_ = other.ptr_;
            incref();         // 增加新对象的引用
        }
        return *this;
    }

    // 移动赋值
    IntrusivePtr& operator=(IntrusivePtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    ~IntrusivePtr() {
        reset();
    }

    // 重置指针，释放当前引用
    void reset() noexcept {
        if (ptr_) {
            ptr_->release(); // 计数减一
            ptr_ = nullptr;
        }
    }

    // 获取裸指针
    T* get() const noexcept { return ptr_; }

    // 重载 -> 和 *，方便访问对象成员
    T* operator->() const noexcept { return ptr_; }
    T& operator*() const noexcept { return *ptr_; }

    explicit operator bool() const noexcept { return ptr_ != nullptr; }
};
```

侵入式引用计数只有一次内存分配（对象本身），引用计数的访问不需要跳转到另一个内存位置（缓存更友好）；但是对象必须继承引用计数基类（侵入性），不方便管理已有类型的对象（比如标准库类型），而且引用计数的线程安全性需要自行决定。

### `scope_guard` 

`scope_guard` 的思想是：把一个可调用对象绑定到一个栈对象的析构函数上——作用域退出时，自动调用。

没有 `scope_guard` 时，可能需要多个返回路径，增加可能抛出异常的调用，需要检查所有的检查点是否遗漏：

```c++
void bad_function()
{
    Resource *res = acquire();
    if (error1)
    {
        release(res);
        return;
    }
    if (error2)
    {
        release(res); // 重复代码
        return;
    }
    release(res); 
}
```

使用 `scope_guard` 后可以只编写一次清理代码：

```c++
void good_function()
{
    Resource *res = acquire();
    auto guard = make_scope_guard([&]()
                                  { release(res); });

    // 中间无论如何返回，或者发生异常，release 都会自动调用
    if (error1)
        return;
    if (error2)
        return;
}
```

## 3. `Lambda` 和函数式编程

### `Lambda` 表达式

Lambda 表达式是一个可以**在使用处就地定义的匿名函数对象**——不用跳到文件头部去声明，不用给编译器生成额外的符号，逻辑就写在调用点旁边。

Lambda 的表达式为：

```c++
[capture](parameters) -> return_type { body }
```

`capture` 是捕获列表，决定了 Lambda 怎么访问外层作用域的变量；

`parameters` 和普通函数的参数列表完全一致；

`-> return_type` 是尾置返回类型，在 C++11 中需要满足特定条件才能省略让编译器推导；

`body` 就是函数体。

```c++
// 不返回
auto do_nothing = []() {};

// 返回一个值
auto forty_two = []() { return 42; };

// 带参数返回
auto double_it = [](int x) { return x * 2; };
int result = double_it(21); 
```

---

**返回类型推导：**

C++ 11 中，只有 Lambda 体中满足以下条件时，编译器才能自动推导返回类型：

- 函数体只有一条 `return` 语句；
- 所有 `return` 语句返回的表达式推导出相同类型；

```c++
// 自动推导为 int
auto square = [](int x) { return x * x; }; 
```

如果函数体比较复杂，比如有多个分支各自返回不同的路径，编译器可能无法推导，或者推导的结果和预期不一致。此时需要显式指定返回类型：

```c++
auto classify = [](int x) -> int {
    if (x > 0) {
        return x * 2;
    } else if (x < 0) {
        return -x;
    }
    return 0;   
};
```

Lambda 表达式最常见的场景是作为 STL 算法的谓词或操作函数。可以避免传一个全局函数指针或使用函数对象。

---

**捕获外部变量：**

需要让 Lambda 访问外部变量时，需要通过捕获列表显式声明：

```c++
int threshold = 50;

// 编译错误：threshold 不在 lambda 的作用域内
// auto check = [](int value) { return value > threshold; };

// 值捕获：复制一份 threshold 到闭包对象中
auto by_value = [threshold](int value) { return value > threshold; };

// 引用捕获：直接引用外部的 threshold
auto by_ref = [&threshold](int value) { return value > threshold; };
```

值捕获会在 Lambda 创建的那一刻复制变量，之后外部的修改不会影响 lambda 内部的副本；引用捕获则让 Lambda 直接操作原始变量。

默认捕获写法：`[=]` 表示值捕获所有用到的外部变量，`[&]` 表示引用捕获所有用到的外部变量。

---

**`Lambda` 类型：**

每一个 Lambda 表达式会产生唯一，匿名的类类型，这个类类型有一个 `operator()` 成员函数，参数和返回值和 Lambda 表达式一致。

```c++
auto greet = [](const std::string& name) -> std::string {
    return "Hello, " + name;
};

// 编译器概念上生成的类
struct /* 编译器生成的唯一名字 */ {
    std::string operator()(const std::string& name) const 
    {
        return "Hello, " + name;
    }
};
auto greet = /* 编译器生成类的实例 */{};
```

因此无法写出 `Lambda` 的类型名字，存储的时候要么用 `auto`（编译期类型已知），要么用 `std::function`（运行时类型擦除，有额外开销）。

### `Lambda` 捕获机制

---

**值捕获：**

在 Lambda 创建时，被捕获的变量被复制，作为闭包类型的成员变量存储，此后外部变量的任何修改不会影响 Lambda 内部的副本。

```c++
void demo_value_capture()
{
    int threshold = 100;

    // threshold 被复制到闭包对象中
    auto is_high = [threshold](int value)
    {
        return value > threshold;
    };

    threshold = 200;            
    bool result = is_high(150); // lambda 里的 threshold 还是 100
}
```

在编译器内部，Lambda 表达式翻译为以下的闭包类型：

```c++
struct ClosureType {
    int threshold;  // 被捕获的变量变成了成员

    bool operator()(int value) const {
        return value > threshold;
    }
};

auto is_high = ClosureType{100};  // 构造时复制 threshold
```

值捕获的成员在 `operator()` 内部默认 `const` ，因此不能修改，如果需要在 Lambda 内部修改捕获的副本，需要加上 `mutable` 关键字：

```c++
int counter = 0;

// counter 在 lambda 内是 const int，不可修改
// auto bad = [counter]() { counter++; };

// 通过 mutable 允许修改 lambda 内部的副本
auto make_counter = [counter]() mutable {
    return ++counter;   // 修改的是闭包对象自己的 counter，不是外部的
};

std::cout << make_counter() << "\n";  // 1
std::cout << make_counter() << "\n";  // 2
std::cout << counter << "\n";         // 0
```

---

**引用捕获：**

编译器在闭包类型中存储的是被捕获变量的指针（引用），Lambda 内部对捕获变量的读写，实际上是对原始变量的操作。

```c++
void demo_ref_capture() {
    int sum = 0;

    auto accumulate = [&sum](int value) {
        sum += value;   // 直接修改外部的 sum
    };

    accumulate(10);
    accumulate(20);
    accumulate(30);	// sum = 60
}
```

在编译器内部，Lambda 表达式翻译为以下的闭包类型：

```c++
struct ClosureType {
    int& sum;  // 存储的是引用

    void operator()(int value) const {
        sum += value;  // 通过引用修改外部变量
    }
};
```

> `operator()` 是 `const` 的，却通过 `sum` 修改了外部变量。这是因为引用本身（存储的地址）是 `const` 的——不能让引用指向另一个对象，但引用所绑定的对象的值是可以修改的。

引用捕获最大的优势是零拷贝，但是需要注意，被引用变量的生命周期必须长于 Lambda。

---

**默认捕获：**

`[=]` 默认值捕获不会捕获 `this` 指针（C++ 20 之前可以），在 C++ 20 之前可能会导致捕获到 `this` 指针。

代码中应当尽量避免默认捕获。

---

**初始化捕获（C++ 14）：**

捕获列表可以写为：`name = expression`，其中 `name` 是一个新的变量名，`expression` 是初始化表达式。

```c++
void demo_init_capture() {
    int base = 10;

    // 捕获 base + 5 的结果，而不是 base 本身
    auto lam = [value = base + 5]() {
        return value * 2;   // value = 15
    };
}
```

初始化捕获可以用于移动捕获：

```c++
#include <memory>

auto make_handler() {
    auto ptr = std::make_unique<int>(42);

    // 把 unique_ptr 移入 lambda
    return [p = std::move(ptr)]() {
        return *p;   // p 是 lambda 独占的
    };
}
```

另一种用法是替代 `mutable` 计数器：

```c++
auto counter_new = [count = 0]() mutable { return ++count; };
```

---

**`*this` 捕获：**

`[this]` 捕获可能会造成悬垂引用（如果 Lambda 的生命周期长于对象本身）。C++ 17 引入 `[*this]` 捕获，其按值捕获整个对象，也就是在闭包类型中存储一份对象副本。

---

**悬垂引用/生命周期：**

```c++
auto make_safe() {
    int count = 0;
    return [count]() mutable { return ++count; };    	// 值捕获
}

auto make_unsafe() {
    int count = 0;
    return [&count]() { return ++count; };				// 引用捕获，但是 count 在离开 make_unsafe 后销毁，这导致返回的 Lambda 闭包对象形成悬垂引用
}
```

---

对于小型不可变数据（`int`、`float`、简单结构体），值捕获是最安全的默认选择。它确保 Lambda 不依赖外部状态，线程安全，也不会有生命周期问题。

对于大型对象（`std::vector`、`std::string`），如果 Lambda 内部需要读取而不修改，引用捕获加上 `const` 是零拷贝的方案；

如果 Lambda 需要独立持有这个对象，用初始化捕获 `name = std::move(obj)` 把它移入闭包。

对于需要在 Lambda 内部修改的外部变量（累加器、状态更新），引用捕获是最自然的选择，但要确保变量的生命周期足够长。

### 泛型/模板 `Lambda`

Lambda 的参数类型可以使用 `auto`（C++ 14），对于调用者，类似于模板函数——不同类型的参数会各自实例化一份 `operator()`：

```c++
auto add = [](auto a, auto b) { return a + b; };

// 编译器生成的（简化）
struct ClosureType {
    template<typename T1, typename T2>
    auto operator()(T1 a, T2 b) const {
        return a + b;
    }
};
```

C++ 20 加入了显式模板参数列表，以便对模板参数施加约束：

```c++
auto add_explicit = []<typename T>(T a, T b) {
    return a + b;
};
```

模板参数名可以在 Lambda 体内自由使用。

模板可以接收特定概念的类型：

```c++
// 只接受整数类型
auto int_only = []<std::integral T>(T a, T b) {
    return a + b;
};

// 只接受浮点类型
auto float_only = []<std::floating_point T>(T a, T b) {
    return a + b;
};

// 自定义概念：支持序列化类型
// 类型 T 的对象 t 是否可调用 serialize(t, os)
// 用返回值类型必须为 void（通过 std::same_as<void> 约束）
template<typename T>
concept Serializable = requires(T t, std::ostream& os) {
    { serialize(t, os) } -> std::same_as<void>;
};

auto serialize_and_log = []<Serializable T>(const T& obj) {
    std::ostringstream oss;
    serialize(obj, oss);
    std::cout << "Serialized: " << oss.str() << "\n";
};
```

### 可调用对象

可调用对象是可以用 `()` 调用的东西。

- 普通函数
- 函数指针
- 函数对象
- Lambda 表达式
- 成员函数指针

这些可调用对象的语法各不相同，如果需要泛型，需要做很多模板特化。

---

**`std::function`- 类型擦除函数容器**

`std::function` 是 C++11 引入的通用函数包装器，定义在 `<functional>` 头文件中。它可以存储、复制和调用任何匹配签名要求的可调用对象。核心能力是**把不同类型的可调用对象统一成同一种类型**。

```c++
#include <functional>
#include <iostream>

int add(int a, int b) { return a + b; }

struct Multiplier {
    int factor;
    int operator()(int x) const { return x * factor; }
};

void demo_std_function() {
    std::function<int(int, int)> func;

    // 存储普通函数
    func = add;
    std::cout << func(3, 4) << "\n";   

    // 存储 lambda
    func = [](int a, int b) { return a * b; };
    std::cout << func(3, 4) << "\n";   

    // 存储函数对象
    func = Multiplier{5};
    std::cout << func(10) << "\n";     // 签名不匹配
}
```

`std::function` 通过类型擦除实现：`std::function` 内部定义了一个抽象基类（Concept），持有纯虚函数 `invoke`；然后为每种具体的可调用类型生成一个派生类（Model），在派生类中实现 `invoke`。`std::function` 持有一个指向 Concept 的指针，调用时通过虚函数分派到具体实现。

实际的 `std::function` 实现使用了小对象优化，在 `std::function` 对象内部预留一块固定大小的缓冲区，如果被包装的可调用对象足够小，就直接存放在这个缓冲区里，不需要堆分配。

---

**`std::invoke` - 统一调用接口**

C++17 引入的 `std::invoke`（定义在 `<functional>` 中）是一个万能调用器。不管可调用对象是什么类型——普通函数、成员函数指针、Lambda、仿函数，`std::invoke` 都能用同一个语法调用。

```c++
#include <functional>
#include <iostream>

struct Widget {
    void greet(const std::string& msg) {
        std::cout << "Widget says: " << msg << "\n";
    }
    int data = 42;
};

void free_func(int x) {
    std::cout << "free_func: " << x << "\n";
}

void demo_invoke() {
    Widget w;

    // 普通函数
    std::invoke(free_func, 42);

    // 仿函数 / lambda
    std::invoke([](int x) { std::cout << "lambda: " << x << "\n"; }, 99);

    // 成员函数指针 + 对象
    std::invoke(&Widget::greet, w, "hello");

    // 成员变量指针 + 对象（可以读取和修改）
    int val = std::invoke(&Widget::data, w);
    std::invoke(&Widget::data, w) = 100;
}
```

## 4. 类型安全

### `enum class`

C 语言的 `enum` 存在以下问题：

- 可以隐式转换为整型；
- 所有枚举值暴露在外部作用域中，重名时会产生冲突；
- 无法前向声明；

C++ 11 的 `enum class` 针对上述问题给出解决方案：

1. 作用域隔离

   `enum class` 的枚举值不会泄露到外接作用域，必须通过 `EnumName::Value` 的方式访问：

   ```c++
   enum class Color { Red, Green, Blue };
   
   Color c = Color::Red;
   ```

2. 禁止隐式转换

   `enum class` 不会隐式转换为任何整数类型。必须用 `static_cast` 显式转换：

   ```c++
   enum class Color : uint8_t { Red, Green, Blue };
   
   int x = static_cast<int>(Color::Red);
   ```

3. 指定底层类型和前向声明

   `enum class` 可以指定底层类型，并且默认为 `int`。指定底层类型后，编译器在声明时就知道枚举的大小。

   ```c++
   // status.h —— 前向声明
   enum class Status : uint8_t;
   
   // device.h —— 只需要前向声明
   class Device {
   public:
       Status get_status() const;
       void set_status(Status s);
   };
   
   // status.cpp —— 完整定义
   enum class Status : uint8_t { kOk = 0, kError = 1, kBusy = 2 };
   ```

   在头文件中只需要前向声明，完整定义放在 `.cpp` 文件中，这就打破了头文件之间的循环依赖。

C++ 中，由于禁止隐式转换，使用枚举类型进行位运算需要重载运算符：

```c++
#include <type_traits>

enum class Permission : uint32_t {
    kNone    = 0,
    kRead    = 1 << 0,
    kWrite   = 1 << 1,
    kExecute = 1 << 2
};

// 辅助函数：枚举值到底层类型的转换
template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

constexpr Permission operator|(Permission a, Permission b) noexcept
{
    return static_cast<Permission>(to_underlying(a) | to_underlying(b));
}

constexpr Permission operator&(Permission a, Permission b) noexcept
{
    return static_cast<Permission>(to_underlying(a) & to_underlying(b));
}

constexpr Permission operator^(Permission a, Permission b) noexcept
{
    return static_cast<Permission>(to_underlying(a) ^ to_underlying(b));
}

constexpr Permission operator~(Permission a) noexcept
{
    return static_cast<Permission>(~to_underlying(a));
}

constexpr Permission& operator|=(Permission& a, Permission b) noexcept
{
    a = a | b;
    return a;
}

constexpr Permission& operator&=(Permission& a, Permission b) noexcept
{
    a = a & b;
    return a;
}

// 辅助判断：是否有任何标志位被设置
constexpr bool has_any_flag(Permission flags) noexcept
{
    return to_underlying(flags) != 0;
}

// 辅助判断：是否包含特定标志位
constexpr bool has_flag(Permission flags, Permission flag) noexcept
{
    return to_underlying(flags & flag) != 0;
}
```

由于 `enum class` 的值必须通过限定名访问，编译器知道所有可能的取值，可以在遗漏分支时发出警告。在使用 `enum class` 做 `switch` 时，不要写 `default` 分支。

C++ 20 引入了 `using enum` 声明，可以一次性把某个枚举的所有值引入当前作用域。

```c++
enum class TokenType {
    kNumber, kString, kIdentifier,
    kPlus, kMinus, kStar, kSlash,
    kLeftParen, kRightParen, kEof
};

std::string_view token_to_string(TokenType type)
{
    // 把所有枚举值引入函数作用域
    using enum TokenType;

    switch (type) {
    case kNumber:     return "number";
    case kString:     return "string";
    case kIdentifier: return "identifier";
    case kPlus:       return "+";
    case kMinus:      return "-";
    case kStar:       return "*";
    case kSlash:      return "/";
    case kLeftParen:  return "(";
    case kRightParen: return ")";
    case kEof:        return "eof";
    }
    return "unknown";
}
```

`using enum` 的作用域仅限于当前块（花括号内），所以不会污染外部作用域。需要注意，如果两个枚举有同名的值，同时 `using enum` 会产生冲突。

### `typedef`

普通的 `typedef` 和 `using` 只是给类型起一个别名，并不会区分使用同一类型的不同变量。

解决方法是使用 phantom type 模式：用一个只有标记作用、不占实际空间的模板参数来区分不同的类型。

```c++
// 标签结构体, 用于区分类型, 不提供实现
struct WidthTag {};
struct HeightTag {};

// 类型包装器
template <typename Tag, typename Rep = int>
class StrongInt {
public:
    
    // 构造函数
    constexpr explicit StrongInt(Rep value) : value_(value) 
    {
        
    }
    
    // 读取值
    constexpr Rep get() const noexcept 
    { 
        return value_; 
    }

private:
    Rep value_;
};

using Width  = StrongInt<WidthTag>;
using Height = StrongInt<HeightTag>;

Width w(100);
Height h(200);
```

此时 `Width` 和 `Height` 是两种不同类型，编译器阻止把一个值赋给另一个值。

`WidthTag` 和 `HeightTag` 是空的类，不占用任何存储空间，编译器在生成代码时，`StrongInt<WidthTag>` 和 `StrongInt<HeightTag>` 的运行时表现和裸 `int` 完全一样。

强类型包装器可以使用函数重载实现各类运算操作。

可以使用推导指引简化构造：

```c++
// 对于 Rep 类型的推导指引
template <typename Tag>
StrongInt(Tag*) -> StrongInt<Tag, int>;

// 使用时只需要指定 Tag
struct ScoreTag {};
using Score = StrongInt<ScoreTag>;
```

实际应用中，可以用 `constexpr` 和 `auto` 推导让模板代码更加自然：

```c++
template <typename Tag, typename Rep>
constexpr auto make_strong(Rep value)
{
    return StrongInt<Tag, Rep>(value);
}

auto width = make_strong<WidthTag>(100);
```

### `std::variant`

C 语言的 `union` 存在以下问题：

- `union` 不记录当前持有的是哪个成员；
- `union` 不支持带有非平凡构造/析构函数的类型，比如 `std::string`。

`std::variant` 解决了以上问题。

---

**构造和赋值：**

`std::variant<Types...>` 可以在同一时刻持有 `Types...` 中恰好一种类型的值。默认构造时，它会构造第一个备选类型。

```c++
#include <variant>
#include <string>
#include <iostream>

int main()
{
    // 默认构造：持有 int（第一个备选），值为 0
    std::variant<int, double, std::string> v;

    // 赋值：自动切换到对应类型
    v = 42;                        // 持有 int
    v = 3.14;                      // 持有 double
    v = std::string("hello");      // 持有 std::string

    // 构造时直接指定
    std::variant<int, std::string> v2 = std::string("world");
}
```

每次赋值时，`variant` 会自动销毁旧值、构造新值。

---

**访问值：**

访问 `variant` 的值有三种方式：

```c++
std::variant<int, double, std::string> v = 3.14;

// std::get<T> —— 类型不匹配时抛出 std::bad_variant_access
double d = std::get<double>(v);   // OK
// int bad = std::get<int>(v);    // 抛出异常！

// std::get_if<T> —— 不抛异常，返回指针
if (auto* ptr = std::get_if<double>(&v)) {
    std::cout << "double: " << *ptr << "\n";
}

// std::holds_alternative<T> —— 只检查类型
if (std::holds_alternative<double>(v)) {
    std::cout << "it's a double\n";
}
```

如果只需要检查类型，用 `std::holds_alternative`；如果需要获取值的指针（且不想处理异常），用 `std::get_if`；如果确定类型是对的并且希望不匹配时立刻报错，用 `std::get`。

---

**`std::visit` 和访问者模式：**

`std::visit` 接受一个可调用对象 `visitor` 和若干个 `variant` 对象，根据根据 `variant` 当前持有的类型来分派调用。

```c++
std::variant<int, double, std::string> v = std::string("hello");

// auto&& 是万能引用, visit 根据 v 持有的类型实例化 Lambda
std::visit([](auto&& arg) {
    std::cout << arg << "\n";
}, v);
```

如果不同类型需要用不同的逻辑进行处理，需要构建一个重载集合：一个对每种备选类型都有对应重载的可调用对象。

```c++
template <class... Ts>				// 定义模板类型集合
struct Overloaded : Ts... {			// 从所有类型参数公开继承
    using Ts::operator()...;  		// 折叠表达式, 将每个基类的 operator() 引入当前作用域
};

template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;	// 调用时进行推导指引

std::variant<int, double, std::string> v = 3.14;

std::visit(Overloaded{
    [](int i)         { std::cout << "int: " << i << "\n"; },
    [](double d)      { std::cout << "double: " << d << "\n"; },
    [](const std::string& s) { std::cout << "string: " << s << "\n"; }
}, v);
```

编译器会检查 `Overloaded` 是否覆盖了 `variant` 的所有备选类型。如果漏掉了某个类型的处理，编译器会直接报错。

`visitor` 可以返回值，所有 Lambda 的返回类型必须兼容：

```c++
std::variant<int, double, std::string> v = 42;

auto type_name = std::visit(Overloaded{
    [](int)    -> std::string { return "int"; },
    [](double) -> std::string { return "double"; },
    [](const std::string&) -> std::string { return "string"; }
}, v);
```

---

**`std::variant` 和运行时多态：**

`std::variant` 可以替代虚函数实现多态。传统的虚函数多态需要堆分配、虚函数表指针、引用语义——而 `variant` 可以直接在栈上存储值，没有虚函数调用开销。

```c++
struct Circle {
    double radius;
    explicit Circle(double r) : radius(r) {}
};

struct Rectangle {
    double width, height;
    Rectangle(double w, double h) : width(w), height(h) {}
};

using Shape = std::variant<Circle, Rectangle>;

double area(const Shape& s)
{
    return std::visit(Overloaded{
        [](const Circle& c)    { return 3.14159 * c.radius * c.radius; },
        [](const Rectangle& r) { return r.width * r.height; }
    }, s);
}

// variant 是值语义, 直接在栈上存储
std::vector<Shape> shapes;
shapes.push_back(Circle(5.0));
shapes.push_back(Rectangle(3.0, 4.0));

for (const auto& s : shapes) {
    std::cout << area(s) << "\n";
}
```

`variant` 方式的优势在于：值语义（不需要 `new`/`delete`）、连续内存（`vector` 中直接存储，缓存友好）、编译期类型检查（所有 `visit` 的分支都在编译期确定）。

但是如果新增派生类，必须修改 `variant` 定义，这是不灵活的。如果第三方可以扩展新类型，虚函数仍然是更好的选择。

### `std::optional`

`optional` 用于表示可能没有值。`std::optional<T>` 表示要么持有一个 `T` 类型的值，要么什么都没有。它是一个值类型（不是指针），持有的对象直接嵌套在 `optional` 内部的存储中——没有动态内存分配。

```c++
#include <optional>
#include <string>
#include <iostream>

std::optional<int> a;                      // 空（不持有值）
std::optional<int> b = 42;                 // 持有 42
std::optional<int> c = std::nullopt;       // 显式空
std::optional<std::string> d = "hello";    // 持有 "hello"
```

检查和访问如下：

```c++
std::optional<int> opt = 42;

// 检查是否有值
if (opt.has_value()) { /* ... */ }
if (opt) { /* ... */ }             			// 等价的隐式 bool 转换

// 访问值
int x = *opt;                       		// 解引用(未检查——空时是 UB)
int y = opt.value();                		// 空时抛 std::bad_optional_access
int z = opt.value_or(0);            		// 空时返回默认值 0

// 访问成员(对于类)
std::optional<std::string> name = "Alice";
if (name) {
    std::cout << "length: " << name->size() << "\n";  // operator->
}
```

已经检查过 `has_value()` 的代码路径中，使用 `*opt` ，性能更好而且语义清晰。在没有检查的情况下，`value()` 更安全 —— 抛异常而不是 UB。实际应用中更推荐使用 `value_or()`。

`optional<T>` 和 `T*` 都能表达可能没有值，`optional<T>` 是值语义，持有（或打算持有）一个完整的 `T` 对象，拷贝 `optional` 会拷贝 `T` 的值（如果有值的话），析构 `optional` 会析构 `T`。`T*` 是引用语义，指向某个外部的 `T` 对象（或者为空）。拷贝指针只是拷贝地址，不会拷贝对象本身。

如果需要表达值可能存在也可能不存在，用 `optional`；如果需要表达指向某个外部对象的可空引用，用指针。

`optional` 最常见的用途是作为函数返回值 —— 函数可能返回一个有效值，也可能返回无值。调用方必须在类型系统层面处理无值的情况。

`optional` 也可以用作函数参数，表示参数是可选的。

## 5. `auto` 和 `decltype`

### `auto` 类型推导

`auto` 的推导规则和模板参数推导规则一致。在 `auto x = expr` 中，编译器把 `auto` 作为模板参数 `T`，用 `expr` 的类型推导 `T`。

```c++
auto x = 42;		// int
auto y = 3.14;		// double
auto z = "hello";	// const char*
auto flag = true;	// bool
```

`auto` 会丢弃引用和顶层 `const`。

```c++
const int ci = 42;
auto a = ci;		// int(无 const)

int val = 10;
int& ref = val;
auto b = ref;		// int(丢弃引用, 拷贝)
```

顶层 `const` 指变量本身为 `const`，底层 `const` 指所指向的对象是 `const`。

```c++
const int* p = nullptr;   // 底层 const(指针指向的内容是 const)
auto q = p;               // const int*(保留底层 const)

int* const p2 = nullptr;  // 顶层 const(指针本身是 const)
auto q2 = p2;             // int*(丢弃顶层 const)
```

---

**`auto` 的写法：**

`auto` —— 按值拷贝，产生一个拷贝。

```c++
auto x = func();		// 拷贝返回值
```

`auto&` —— 左值引用，绑定到左值，并且可以修改源对象。不可绑定到右值（临时对象）：

```c++
std::vector<int> v = {1, 2, 3};
auto& first = v[0];		// int& 可以修改 v[0];
first = 100;
```

`const auto&` —— `const` 左值引用，只读访问，不拷贝。`const` 引用可以绑定右值以延长生命周期。

```c++
const auto& name = get_string();
```

`auto&&` —— 转发引用

使用右值初始化时，是右值引用；使用左值初始化时，是左值引用。

```c++
int x = 42;
auto&& r1 = x;          	// int&（左值初始化，推导为 int&）
auto&& r2 = 42;         	// int&&（右值初始化，推导为 int&&）
auto&& r3 = get_value(); 	// 取决于返回值类型
```

---

C++ 17 前，`auto x = {1, 2, 3};` 会被推导为 `std::initializer_list<int>`。

```c++
auto x1 = {1, 2, 3};      // std::initializer_list<int>
auto x2 = {1, 2.0};       // 编译错误：元素类型不一致
```

C++ 17 后，单个元素直接推导为元素的类型，多个元素直接编译错误。

---

`std::vector<bool>` 为了节省空间将 bool 打包为位，因此其 `operator[]` 不返回 `bool&` 而是返回代理类型 `std::vector<bool>::reference`，此时 `auto&` 返回的是代理类型的引用。为解决此问题，可以用 `auto` 进行按值拷贝。

Eigen 等数学库的表达式模板、某些 range adapter 的迭代器也返回代理类型。

---

**`auto` 作为返回类型：**

C++ 14 允许返回类型用 `auto` 声明，编译器根据 `return` 语句推导返回类型，此时所有 `return` 语句必须推导出相同的类型。

```c++
auto add(int a, int b) {
    return a + b;  
}
```

C++ 11 中，如果返回类型依赖参数类型，需要用尾置返回类型：

```c++
template<typename T, typename U>
auto add(T t, U u) -> decltype(t + u) {
    return t + u;
}
```

---

**和 `using` 类型别名配合：**

`using` 通常用于给复杂类型起一个可读名称，`auto` 用于在局部简化代码。

`using` 是 `typedef` 的替代，语法更加直观且支持模板别名：

```c++
using handler_t = void(*)(int, void*);
using map_iter_t = std::map<int, std::string>::iterator;

template<typename T>
using Vec = std::vector<T>;

Vec<int> v1 = {1, 2, 3};   
```

### `decltype` 和返回类型

`auto` 根据初始化表达式推导新变量的类型；而 `decltype` 查询一个已有表达式的类型（保留引用和 `const`）。

---

**`decltype` 推导规则：**

1. `decltype(variable)` 和 `decltype((variable))`：

   对于不加括号的变量名，`decltype` 返回该变量声明时的类型：

    ```c++
    int x = 42;
    decltype(x) a = 100;      // int
    
    const int& cr = x;
    decltype(cr) b = x;        // const int&
    ```

   对于加括号的变量名，返回的是变量作为一个表达式的类型，结果是左值引用：

   ```c++
   int x = 42;
   decltype((x)) c = x;       // int&
   ```

2. `decltype` 推导函数调用表达式

   `decltype` 的操作数是函数调用表达式时，返回函数返回值的精确类型。

   ```c++
   int& get_ref() {
       static int x = 42;
       return x;
   }
   
   int get_val() {
       return 42;
   }
   
   decltype(get_ref()) a = get_ref();  // int&
   decltype(get_val()) b = get_val();  // int
   ```

3. `decltype` 推导表达式

   `decltype` 根据表达式的值类别决定类型。如果表达式是左值，结果是引用；如果表达式是右值，结果是非引用。

   ```c++
   int x = 42;
   
   decltype(x + 1) a = 0;    // int（x + 1 是右值）
   decltype(x = 10) b = x;   // int&（赋值表达式返回左值引用）
   decltype(++x) c = x;      // int&（前置 ++ 返回左值引用）
   decltype(x++) d = 0;      // int（后置 ++ 返回右值）
   ```

---

**`decltype(auto)`：**

编译器使用 `decltype` 的规则推导 `auto` 部分。

```c++
int x = 42;

auto a = (x);            // int  (auto 丢弃引用)
decltype(auto) b = (x);  // int& (decltype 保留引用)
decltype(auto) c = x;    // int
```

如果需要完美转发返回值的引用语义时，可以使用 `decltype(auto)`：

```c++
class Container {
public:
    decltype(auto) operator[](std::size_t index) {
        return data_[index];  // data_[int] 返回 int&，decltype(auto) 保留
    }

    decltype(auto) operator[](std::size_t index) const {
        return data_[index];  // const 版本返回 const int&
    }

private:
    std::vector<int> data_;
};
```

注意，如果返回局部变量的引用并且使用 `decltype(auto)` 时，会导致未定义行为。

```c++
decltype(auto) get_value() {
    int x = 42;
    return (x);   // 返回 int&, 但 x 在函数结束后销毁 -> 悬空引用
}
```

---

**尾置返回类型：**

在 C++ 11 中，函数的返回类型如果依赖参数类型，就必须用尾置返回类型；C++14 允许直接用 `auto` 做返回类型，编译器从 `return` 语句推导。如果需要精确保留引用语义，仍然需要使用 `decltype` 或 `decltype(auto)`。

C++ 11 的 Lambda 如果返回类型不能自动推导，需要显式指定尾置返回类型；C++ 14 之后，Lambda 的返回类型几乎总能自动推导，不再需要显式指定。

---

**`decltype` 和模板：**

`decltype` 在模板中最常见的用途是实现完美转发返回值——让包装函数返回和被包装函数完全相同的类型。

```c++
template<typename Callable, typename... Args>
decltype(auto) perfect_forward(Callable&& f, Args&&... args) {
    return std::forward<Callable>(f)(std::forward<Args>(args)...);
}
```

### 类模板参数推导

在 C++ 17 之前，每次实例化**类模板**都需要写全模板参数。

```c++
std::pair<int, double> p(1, 2.0);   
```

C++ 17 引入了类模板参数推导：CTAD，编译器从构造函数的参数自动推导模板参数，不需要手动指定。

```c++
std::pair p(1, 2.0);  	// 推导为 std::pair<int, double>
```

`std::vector` 有一个特殊的推导指引，从迭代器对推导元素类型：

```c++
std::vector v1 = {1, 2, 3};                    // std::vector<int>
std::vector v2(v1.begin(), v1.begin() + 2);    // std::vector<int>
```

`std::unique_ptr` 和 `std::shared_ptr` 不支持从裸指针的 CTAD，因为智能指针的构造函数模板参数推导规则与普通类模板不同 —— 构造函数接受指针类型，但无法从裸指针推导出模板参数。

---

**隐式推导指引：**

CTAD 中，编译器通过推导指引推导模板参数，如果类模板的构造函数使用了所有模板参数，编译器会自动生成隐式推导指引。

```c++
template<typename T, typename U>
struct MyPair {
    T first;
    U second;
    
    MyPair(T f, U s) : first(f), second(s) 
    {
        
    }
};

MyPair p(1, 2.0);  // 隐式推导为 MyPair<int, double>
```

如果一个类模板有多个构造函数，编译器会为每个构造函数生成一个隐式推导指引，当创建对象时，编译器会尝试所有的推导指引，选择最匹配的那个。

> - 隐式推导指引不能推导嵌套的模板参数。
> - 如果构造函数有默认参数，隐式推导指引只考虑没有默认值的参数。带默认值的模板参数不会被自动推导。

---

**自定义推导指引：**

```c++
template<typename ...>
ClassName(params) -> ClassName<deduced types>;
```

一个自定义推导指引的例子：

```c++
template<typename T, std::size_t N>
class FixedBuffer {
public:
    FixedBuffer(std::initializer_list<T> init) {
        std::copy(init.begin(), init.begin() + N, data_.begin());
    }

private:
    std::array<T, N> data_;
};

// 自定义推导指引：从花括号列表推导 T 和 N
template<typename T, typename... Args>
FixedBuffer(T, Args...) -> FixedBuffer<T, 1 + sizeof...(Args)>;

FixedBuffer buf = {1, 2, 3, 4, 5}; 	// 从 1 推导为 int, 其余参数 + 1 为总参数个数
									// FixedBuffer<int, 5>
```

