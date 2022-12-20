#pragma once

#include <functional>
#include <iostream>
#include <unordered_map>
#include <vector>

using Callback = std::function<void()>;

#define DECLARE_SIGNAL_METHODS(name, signal)          \
    int add##name##Callback(const Callback &callback) \
    {                                                 \
        return signal.addCallback(callback);          \
    }                                                 \
    void remove##name##Callback(int id)               \
    {                                                 \
        signal.removeCallback(id);                    \
    }                                                 \
    void clear##name##Callbacks()                     \
    {                                                 \
        signal.clearCallbacks();                      \
    }

class Signal
{
public:
    void invoke()
    {
        for (const auto &c: callbacks_)
            c.second();
    }

    int addCallback(const std::function<void()> &f)
    {
        int id = generate_id();
        callbacks_[id] = f;

        return id;
    }

    void removeCallback(int id)
    {
        callbacks_.erase(callbacks_.find(id));
    }

    void clearCallbacks()
    {
        callbacks_.clear();
    }

private:
    int generate_id()
    {
        int id = 0;
        while (has_id(id))
            id++;
        return id;
    }

    bool has_id(int id)
    {
        return callbacks_.find(id) != callbacks_.end();
    }

private:
    std::unordered_map<int, Callback> callbacks_{};
};

class DestroyNotify
{
public:
    DECLARE_SIGNAL_METHODS(Destroyed, signal_destroyed_);

    virtual ~DestroyNotify()
    {
        signal_destroyed_.invoke();
    }

private:
    Signal signal_destroyed_;
};

