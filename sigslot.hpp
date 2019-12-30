
#include <functional>
#include <list>
#include <memory>

struct trackable
{
    static std::shared_ptr<trackable> get_base_checkable()
    {
        static std::shared_ptr<trackable> base_checkable(std::make_shared<trackable>());
        return base_checkable;
    }
};

template <typename... _ArgTypes>
class sigslot
{
public:
    using SlotFunc = std::function<void(_ArgTypes...)>;
    using SlotID = std::weak_ptr<SlotFunc>;
    using SlotType = std::pair<std::shared_ptr<SlotFunc>, std::weak_ptr<trackable>>;
    SlotID connect(SlotFunc func, std::weak_ptr<trackable> track = trackable::get_base_checkable())
    {
        auto const f = std::make_shared<SlotFunc>(std::move(func));
        m_funcs.push_back(std::make_pair(f, track));
        return f;
    }

    void disconnect(SlotID f)
    {
        m_funcs.remove_if([f](SlotType slot) {
            return slot.first == f.lock();
        });
    }

    template <typename... _Args>
    void operator()(_Args &&... args)
    {
        m_funcs.remove_if([&args...](SlotType slot) {
            if (slot.second.lock())
            {
                (*slot.first)(std::forward<_Args>(args)...);
                return false;
            }
            return true;
        });
    }

private:
    std::list<SlotType> m_funcs;
};
