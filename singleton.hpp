#ifndef SINGLETON_HPP
#define SINGLETON_HPP

template <typename T>
class singleton
{
public:
    static T& instance()
    {
        static T inst;
        return inst;
    }
protected:
    singleton() {}
    ~singleton() {}
public:
    singleton(singleton const &) = delete;
    singleton& operator=(singleton const &) = delete;
};
#endif // !SINGLETON_HPP