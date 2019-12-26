

#include <iostream>
#include "sigslot.hpp"

using namespace std;

class observer
{
public:
    static observer *instance()
    {
        static observer inst;
        return &inst;
    }

    sigslot<int>::SlotID connect(sigslot<int>::SlotFunc slot)
    {
        return m_sig.connect(std::move(slot));
    }

    void notify(int n)
    {
        m_sig(n);
    }

private:
    sigslot<int> m_sig;
};

class subject
{
public:
    void fun(int a)
    {
        cout << "in subject::fun a=" << a << endl;
    }

    void reg()
    {
        //m_id = observer::instance()->connect(std::bind(&subject::fun, this, std::placeholders::_1));
        m_id = observer::instance()->connect([this](int s) {
            fun(s);
        });
    }

private:
    sigslot<int>::SlotID m_id;
};

int main(int, char **)
{
    subject s;
    s.reg();
    observer::instance()->notify(99);
    return 0;
}
