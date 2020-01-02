
#pragma once

#include <memory>
#include <functional>
#include <list>

namespace utils
{

namespace detail
{

template <typename SharedPtr>
struct shared_ptr_traits
{
};

template <typename T>
struct shared_ptr_traits<std::shared_ptr<T>>
{
    typedef std::weak_ptr<T> weak_type;
};

struct shared_ptr_continer_base
{
    virtual operator bool() = 0;
};

template <typename SharedPtrType>
class shared_ptr_continer_imp : public shared_ptr_continer_base
{
public:
    shared_ptr_continer_imp(SharedPtrType const &shared)
        : m_shared(shared)
    {
    }
    operator bool()
    {
        return (bool)m_shared;
    }

private:
    SharedPtrType m_shared;
};

class shared_ptr_continer
{
public:
    shared_ptr_continer() {}
    template <typename SharedPtrType>
    shared_ptr_continer(SharedPtrType const &shared)
        : m_imp(std::make_unique<shared_ptr_continer_imp<SharedPtrType>>(shared))
    {
    }

    operator bool()
    {
        return m_imp && m_imp->operator bool();
    }

private:
    std::unique_ptr<shared_ptr_continer_base> m_imp;
};

struct weak_ptr_continer_base
{
    virtual shared_ptr_continer lock() const = 0;
};

template <typename WeakPtrType>
class weak_ptr_continer_imp : public weak_ptr_continer_base
{
public:
    weak_ptr_continer_imp(WeakPtrType const &weak) : m_weak(weak)
    {
    }
    virtual shared_ptr_continer lock() const
    {
        return shared_ptr_continer(m_weak.lock());
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

    slot(SlotFunc const &func) : m_func(func)
    {
    }

    slot(slot &other)
    {
        m_weak.swap(other.m_weak);
        m_tracked = other.m_tracked;
        m_func = other.m_func;
    }

    template <typename SharedPtrType>
    slot &track(SharedPtrType const &sharedptr)
    {
        auto weak = typename shared_ptr_traits<SharedPtrType>::weak_type(sharedptr);
        m_weak = std::make_unique<weak_ptr_continer_imp<decltype(weak)>>(weak);
        m_tracked = true;
        return *this;
    }

    template <typename... _Args>
    bool operator()(_Args &&... args)
    {
        shared_ptr_continer sharad;
        if (m_tracked)
        {
            sharad = m_weak->lock();
            if (not sharad)
            {
                return false;
            }
        }
        if (!m_func)
        {
            return false;
        }
        m_func(std::forward<_Args>(args)...);
        return true;
    }

private:
    SlotFunc m_func;
    std::unique_ptr<weak_ptr_continer_base> m_weak;
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

    SlotID connect(SlotType &slot)
    {
        auto const s = std::make_shared<SlotType>(slot);
        m_slots.push_back(s);
        return s;
    }

    void disconnect(SlotID const &s)
    {
        m_slots.remove_if([&s](SlotType const &slot) {
            return slot == s.lock();
        });
    }

    template <typename... _Args>
    void operator()(_Args &&... args)
    {
        m_slots.remove_if([&args...](std::shared_ptr<SlotType> const &s) {
            return !(*s)(std::forward<_Args>(args)...);
        });
    }

private:
    std::list<std::shared_ptr<SlotType>> m_slots;
};

} // namespace utils
