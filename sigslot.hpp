
#pragma once

#include <memory>
#include <functional>
#include <list>
#include <mutex>
#include <algorithm>

namespace utils
{

namespace detail
{

template <typename SmartPtr>
struct smart_ptr_traits
{
};

template <typename T>
struct smart_ptr_traits<std::shared_ptr<T>>
{
    typedef std::weak_ptr<T> weak_type;
};

template <typename T>
struct smart_ptr_traits<std::weak_ptr<T>>
{
    typedef std::shared_ptr<T> share_type;
};

struct shared_ptr_continer
{
    virtual operator bool() = 0;
};

template <typename SharedPtrType>
class shared_ptr_continer_imp : public shared_ptr_continer
{
public:
    shared_ptr_continer_imp(SharedPtrType const &shared)
        : m_shared(shared)
    {
    }
    virtual ~shared_ptr_continer_imp(){}
    operator bool()
    {
        return (bool)m_shared;
    }

private:
    SharedPtrType m_shared;
};


struct weak_ptr_continer
{
    virtual std::shared_ptr<shared_ptr_continer> lock() const = 0;
};

template <typename WeakPtrType>
class weak_ptr_continer_imp : public weak_ptr_continer
{
public:
    weak_ptr_continer_imp(WeakPtrType const &weak) : m_weak(weak)
    {
    }
    virtual ~weak_ptr_continer_imp(){}
    virtual std::shared_ptr<shared_ptr_continer> lock() const
    {
        using SharedPtrType = typename smart_ptr_traits<WeakPtrType>::share_type;
        return std::make_shared<shared_ptr_continer_imp<SharedPtrType>>(m_weak.lock());
    }

private:
    WeakPtrType m_weak;
};

template <typename... signature>
class slot;

template <typename R, typename... ArgTypes>
class slot<R(ArgTypes...)>
{
public:
    using SlotFunc = std::function<R(ArgTypes...)>;

    template <typename T>
    slot(T const &func) : m_func(func)
    {
    }

    slot(slot const &other)
    {
        m_weak = other.m_weak;
        m_tracked = other.m_tracked;
        m_func = other.m_func;
    }

    template <typename SharedPtrType>
    slot &track(SharedPtrType const &sharedptr)
    {
        using WeakPtrType = typename smart_ptr_traits<SharedPtrType>::weak_type;
        m_weak = std::make_shared<weak_ptr_continer_imp<WeakPtrType>>(WeakPtrType(sharedptr));
        m_tracked = true;
        return *this;
    }

    template <typename... _Args>
    bool operator()(_Args &&... args)
    {
        if (!m_func)
        {
            return false;
        }
        if (m_tracked)
        {
            auto shared = m_weak->lock();
            if (shared->operator bool())
            {
                m_func(std::forward<_Args>(args)...);
            }
            else
            {
                return false;
            }
            
        }
        else
        {
            m_func(std::forward<_Args>(args)...);
        }

        return true;
    }

private:
    SlotFunc m_func;
    std::shared_ptr<weak_ptr_continer> m_weak;
    bool m_tracked = false;
};

} // namespace detail

template <typename... signature>
class sigslot;

template <typename R, typename... ArgTypes>
class sigslot<R(ArgTypes...)>
{
public:
    using SlotType = detail::slot<R(ArgTypes...)>;
    using SlotID = std::weak_ptr<SlotType>;

    SlotID connect(SlotType const &slot)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        auto const s = std::make_shared<SlotType>(slot);
        m_slots.push_back(s);
        return s;
    }

    void disconnect(SlotID const &s)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_slots.remove_if([&s](std::shared_ptr<SlotType> const &slot) {
            return slot == s.lock();
        });
    }

    template <typename... _Args>
    void operator()(_Args &&... args)
    {
        m_mutex.lock();
        auto slots = m_slots;
        m_mutex.unlock();

        decltype(slots) remove_slots;
        std::copy_if(slots.begin(), slots.end(), std::back_inserter(remove_slots), [&args...](std::shared_ptr<SlotType> const &s) {
            return !(*s)(std::forward<_Args>(args)...);
        });
        if (remove_slots.empty())
        {
            return;
        }

        m_mutex.lock();
        m_slots.remove_if([&remove_slots](std::shared_ptr<SlotType> const &slot) {
            return std::find(remove_slots.begin(), remove_slots.end(), slot) != remove_slots.end();
        });
        m_mutex.unlock();
    }

private:
    std::list<std::shared_ptr<SlotType>> m_slots;
    std::mutex m_mutex;
};

} // namespace utils
