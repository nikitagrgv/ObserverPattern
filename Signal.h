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

#define DECLARE_SIGNAL_METHODS_KEEP(name, signal)      \
    CallbackKeeper add##name##CallbackK(const Callback &callback) \
    {                                                  \
        return signal.addCallback(callback);           \
    }                                                  \
    void remove##name##CallbackK(int id)               \
    {                                                  \
        signal.removeCallback(id);                     \
    }                                                  \
    void clear##name##CallbacksK()                     \
    {                                                  \
        signal.clearCallbacks();                       \
    }

class Signal final
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

// TODO test
class CallbackKeeper final
{
    friend class Signal;

public:
    CallbackKeeper(const CallbackKeeper&) = delete;
    CallbackKeeper& operator=(const CallbackKeeper&) = delete;

    CallbackKeeper(CallbackKeeper &&other) noexcept
        : is_removed_(other.is_removed_),
          signal_(other.signal_),
          id_(other.id_)
    {
        other.is_removed_ = true;
        other.signal_ = nullptr;
        other.id_ = -1;
    }

    CallbackKeeper& operator=(CallbackKeeper&& other) = delete; // TODO do later

    void remove()
    {
        if (!is_removed_)
        {
            signal_->removeCallback(id_);
            is_removed_ = true;
        }
    }

    ~CallbackKeeper()
    {
        remove();
    }

private:
    CallbackKeeper(Signal &signal, int id) : signal_(&signal), id_(id) {}

private:
    bool is_removed_{false};

    int id_{};
    Signal* signal_{nullptr};
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

