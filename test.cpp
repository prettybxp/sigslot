

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

class subject : public enable_shared_from_this<subject>
{
public:
    void fun(int a)
    {
        cout << "in subject::fun a=" << a << endl;
    }

    void reg()
    {
        m_id = observer::instance()->connect([self = (std::weak_ptr<subject>)shared_from_this()](int s) {
            if (auto shared_self = self.lock())
            {
                shared_self->fun(s);
            }
        });
    }

private:
    sigslot<int>::SlotID m_id;
};

class A : public trackable
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
    sigslot<int> sig;
    //{
    auto a = std::make_shared<A>();
    auto h = sig.connect(std::bind(&A::haha, a.get(), std::placeholders::_1), a);
    //}
    a.reset();
    //sig.disconnect(h);
    sig(99);

    return 0;
}
