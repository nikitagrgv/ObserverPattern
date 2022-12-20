#include "Signal.h"
#include <functional>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

class Subject : public DestroyNotify
{
public:
    ~Subject()
    {
        signal_deleted_.invoke();
    }

    DECLARE_SIGNAL_METHODS(SubjectChanged, signal_changed_);

    int getValue() const { return value_; }
    void setValue(int val)
    {
        value_ = val;
        signal_changed_.invoke();
    }

private:
    int value_{};

    Signal signal_changed_;
    Signal signal_deleted_;
};

class Observer
{
public:
    explicit Observer(const std::string &name) : name_(name) {}

    void subscribeOn(Subject &subject)
    {
        subject_ = &subject;

        id1 = subject.addSubjectChangedCallback([this]() {
            cout << "before callback " << name_.c_str() << endl;
            value_ = subject_->getValue();
            cout << "value = " << value_ << endl
                 << endl;
        });

        id2 = subject.addDestroyedCallback([this]() {
            id1 = -1;// release
            id2 = -1;
        });
    }

    ~Observer()
    {
        if (id1 != -1)
            subject_->removeSubjectChangedCallback(id1);
        if (id2 != -1)
            subject_->removeDestroyedCallback(id2);
    }

    void doSomething()
    {
        subject_->setValue(4);
    }

private:
    int id1{-1};
    int id2{-1};

    std::string name_;
    Subject *subject_{};
    int value_{};
};


int main()
{
    {
        auto sub1 = new Subject();
        auto obs1 = new Observer("obs1");
        obs1->subscribeOn(*sub1);

        delete sub1;
        delete obs1;
    }

    {
        auto sub1 = new Subject();
        auto obs1 = new Observer("obs1");
        obs1->subscribeOn(*sub1);

        delete obs1;
        delete sub1;
    }

    {
        auto sub1 = new Subject();
        auto obs1 = new Observer("obs1");
        obs1->subscribeOn(*sub1);
        auto obs2 = new Observer("obs2");
        obs2->subscribeOn(*sub1);

        delete sub1;
        delete obs1;
    }

    {
        auto sub1 = new Subject();
        auto obs1 = new Observer("obs1");
        obs1->subscribeOn(*sub1);
        auto obs2 = new Observer("obs2");
        obs2->subscribeOn(*sub1);

        delete obs1;
        delete sub1;
    }


    {
        auto sub1 = new Subject();
        auto obs1 = new Observer("obs1");
        obs1->subscribeOn(*sub1);
        auto obs2 = new Observer("obs2");
        obs2->subscribeOn(*sub1);

        delete obs1;
        obs2->doSomething();
        delete sub1;
        delete obs2;
    }

    {
        auto sub1 = new Subject();
        auto obs1 = new Observer("obs1");
        obs1->subscribeOn(*sub1);
        auto obs2 = new Observer("obs2");
        obs2->subscribeOn(*sub1);

        delete obs1;
        obs2->doSomething();
        delete obs2;
        delete sub1;
    }


    return 0;
}
