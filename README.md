# sigslot
通过C++11实现的一种信号和槽机制, 采用和boost::signal2相似的接口.
非常的轻量级, 但却支持了Track的机制

# Features
1, 非常的轻量级,仅一个头文件

2, 弥补了c++11 没有信号和槽的不足

3, 观察者模式的最佳实践

4, 和boost相似的接口,使用方便,对于看过boost接口的人没有学习成本

5, 支持对槽附加一个智能指针对象, 对象析构时能自动断开连接,防止回调死机

# Building test using CMake
mkdir build

cd to build

cmake ..

make


# Usage
## Hello World
接下来展示一个使用sigslot来输出HelloWorld的例子.
我们创建一个信号,这个信号没有参数,返回void.然后我们使用connect函数连接HelloWorld这个函数对象到信号上. 最后我们触发这个信号,就像调用函数一样.
```
struct HelloWorld
{
    void operator()() const
    {
        cout << "Hello, World!" << endl;
    }
};

void test1()
{
    utils::sigslot<void()> sig;
    HelloWorld hello;
    sig.connect(hello);
    sig();
}
```

## 主动断开

我们连接信号和槽后,有时需要断开.我们的槽有时接收了一些事件后就需要断开.然后程序员需要决定在什么时候断开槽的连接.这里我们可以使用
disconnect主动断开连接,通过传入连接时返回的id标识要断开哪一个槽.
```
void test2()
{
    utils::sigslot<void()> sig;
    HelloWorld hello;
    auto id = sig.connect(hello);
    sig();
    sig.disconnect(id); //断开连接
    sig();
}
```

## 连接多个槽

连接一个槽好像没什么意思.我们可以尝试连接多个槽.
```
struct Hello
{
    void operator()() const
    {
        std::cout << "Hello";
    }
};

struct World
{
    void operator()() const
    {
        std::cout << ", World!" << std::endl;
    }
};

void test3()
{
    utils::sigslot<void()> sig;
    Hello hello;
    World world;
    sig.connect(hello);
    sig.connect(world);
    sig();
}
```
## 参数传递
信号可以传递参数给每一个连接的槽.
```
void print_args(float x, float y)
{
    std::cout << "The arguments are " << x << " and " << y << std::endl;
}

void print_sum(float x, float y)
{
    std::cout << "The sum is " << x + y << std::endl;
}

void print_product(float x, float y)
{
    std::cout << "The product is " << x * y << std::endl;
}

void print_difference(float x, float y)
{
    std::cout << "The difference is " << x - y << std::endl;
}

void print_quotient(float x, float y)
{
    std::cout << "The quotient is " << x / y << std::endl;
}

void test4()
{
    utils::sigslot<void(float, float)> sig;

    sig.connect(&print_args);
    sig.connect(&print_sum);
    sig.connect(&print_product);
    sig.connect(&print_difference);
    sig.connect(&print_quotient);

    sig(5., 3.);
}
```

## 自动连接管理

SignalSlot可以自动跟踪连接到槽的附属对象的生命周期.
对于下面这个例子,设想一下当subject析构后,subject::function槽被调用,这时通常会出现一个段错误.sigslots可以使用track函数,跟踪一个对象的生命周期,当跟踪对象的生命结束时,信号和槽的连接将自动断开. 这里track也可以保证在槽的调用期间,附属对象不会被析构.

```
class subject
{
public:
    void function(int c)
    {
        cout << "in subject::function c=" << c << endl;
    }
};

void test5()
{
    using SigSlot = utils::sigslot<void(int)>;
    SigSlot sig;
    auto s = std::make_shared<subject>();
    //注意这里binding的回调函数中传入了s.get()的原生指针.这里不能传入智能指针,否者对象将无法被析构.
    sig.connect(SigSlot::SlotType(std::bind(&subject::function, s.get(), std::placeholders::_1)).track(s));
    sig(99);
    s.reset();
    sig(66);
}
```

# Author
ifbao@qq.com





