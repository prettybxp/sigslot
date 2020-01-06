

#include <iostream>
#include "sigslot.hpp"

using namespace std;

// Hello, World! (Beginner)
// The following example writes "Hello, World!" using signals and slots. First,
// we create a signal sig, a signal that takes no arguments and has a void return value.
// Next, we connect the hello function object to the signal using the connect method.
// Finally, use the signal sig like a function to call the slots, which in turns invokes HelloWorld::operator() to print "Hello, World!".
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

// Slots aren't expected to exist indefinitely after they are connected.
// Often slots are only used to receive a few events and are then disconnected,
// and the programmer needs control to decide when a slot should no longer be connected.
void test2()
{
    utils::sigslot<void()> sig;
    HelloWorld hello;
    auto id = sig.connect(hello);
    sig();
    sig.disconnect(id);
    sig();
}

// Connecting Multiple Slots (Beginner)
// Calling a single slot from a signal isn't very interesting,
// so we can make the Hello, World program more interesting by splitting the work of printing "Hello, World!" into two completely separate slots.
// The first slot will print "Hello" and may look like this:
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

// Slot Arguments (Beginner)
// Signals can propagate arguments to each of the slots they call.
// For instance, a signal that propagates mouse motion events might want to pass along the new mouse coordinates and whether the mouse buttons are pressed.
// As an example, we'll create a signal that passes two float arguments to its slots.
// Then we'll create a few slots that print the results of various arithmetic operations on these values.

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

// Automatic Connection Management (Intermediate)
// SignalSlot can automatically track the lifetime of objects involved in signal/slot connections,
// including automatic disconnection of slots when objects involved in the slot call are destroyed.

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
    sig.connect(SigSlot::SlotType(std::bind(&subject::function, s.get(), std::placeholders::_1)).track(s));
    sig(99);
    s.reset();
    sig(66);
}

//test lamda
void test6()
{
    using SigSlot = utils::sigslot<void(int)>;
    SigSlot sig;
    sig.connect([](int a) {
        cout << "in lamda a= " << a << endl;
    });
    sig(88);
}

int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    return 0;
}