

#include <iostream>
#include "sigslot.hpp"

using namespace std;

class A
{
public:
    void haha(int c)
    {
        cout << "in haha c=" << c << endl;
    }
};

int main(int, char **)
{

    //   observer::instance()->notify(99);
    utils::sigslot<void(int)> sig;
    //{
    auto a = std::make_shared<A>();

    //cout << slot(66) << endl;

    sig.connect(utils::sigslot<void(int)>::SlotType(std::bind(&A::haha, a.get(), std::placeholders::_1)).track(a));
    //a.reset();
    //auto h = sig.connect(slot);
    //}
    //a.reset();
    //sig.disconnect(h);
    sig(99);

    return 0;
}
