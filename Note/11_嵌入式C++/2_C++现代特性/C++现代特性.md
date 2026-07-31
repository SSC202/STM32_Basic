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

## 2. 智能指针和 RAII

RAII（Resource Acquisition Is Initialization）是 C++ 最核心的资源管理思想：**资源的获取放在构造函数里，资源的释放在析构函数里**。

