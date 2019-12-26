
#include <functional>
#include <list>
#include <memory>

template <typename... _ArgTypes>
class sigslot
{
public:
    using SlotFunc = std::function<void(_ArgTypes...)>;
    using SlotID = std::shared_ptr<SlotFunc>;
    SlotID connect(SlotFunc func)
    {
        auto const f = std::make_shared<SlotFunc>(std::move(func));
        m_funcs.push_back(f);
        return f;
    }

    void disconnect(std::shared_ptr<SlotFunc> f)
    {
        m_funcs.remove_if([f](std::weak_ptr<SlotFunc> weakf) {
            if (weakf.expired() || weakf.lock() == f)
            {
                return true;
            }
            return false;
        });
    }

    template <typename... _Args>
    void operator()(_Args &&... args)
    {
        m_funcs.remove_if([&args...](std::weak_ptr<SlotFunc> weakf) {
            if (auto sharedf = weakf.lock())
            {
                (*sharedf)(std::forward<_Args>(args)...);
                return false;
            }
            return true;
        });
    }

private:
    std::list<std::weak_ptr<SlotFunc>> m_funcs;
};
