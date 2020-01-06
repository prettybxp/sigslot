#include "../sigslot.hpp"
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

class oberver
{
public:
    using SigSlot = utils::sigslot<void(std::string)>;
    using SigType = SigSlot::SlotType;
    using SlotID = SigSlot::SlotID;
    static oberver *instance()
    {
        static oberver inst;
        return &inst;
    }

    SlotID attach(SigType const &func)
    {
        return m_signal.connect(func);
    }

    void detach(SlotID const &id)
    {
        m_signal.disconnect(id);
    }

    void notify(std::string const &msg)
    {
        m_signal(msg);
    }

private:
    SigSlot m_signal;
};

class subject : public std::enable_shared_from_this<subject>
{
public:
    ~subject()
    {
        std::cout << "subject dtor" << std::endl;
    }
    void bookMessage()
    {
        oberver::instance()->attach(oberver::SigType([this](std::string const &msg) {
                                        onMessage(msg);
                                    }).track(shared_from_this()));
    }

    void onMessage(std::string const &msg)
    {
        std::cout << "begin onMessage msg = " << msg << std::endl;
        if (msg == "sleep")
        {
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        std::cout << "end onMessage msg = " << msg << std::endl;
    }
};

void testNormal()
{
    auto s = std::make_shared<subject>();
    s->bookMessage();
    oberver::instance()->notify("Hello world");
    s.reset();
    oberver::instance()->notify("error can not print this");
}

void testAbnormal()
{
    auto s = std::make_shared<subject>();
    s->bookMessage();
    auto t = std::thread([]() {
        oberver::instance()->notify("sleep");
    });
    std::this_thread::sleep_for(std::chrono::seconds(1));
    s.reset();
    t.join();
}

int main()
{
    //testNormal();
    testAbnormal();

    return 0;
}