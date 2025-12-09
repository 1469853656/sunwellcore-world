#ifndef SUNWELL_SINGLETON_H
#define SUNWELL_SINGLETON_H

namespace Sunwell
{
template<typename T>
class Singleton
{
public:
    static T* instance()
    {
        static T _ins {};
        return &_ins;
    }

    Singleton<T>() {};
    Singleton<T>(const Singleton<T>&) = delete;
    Singleton<T>(Singleton<T>&&) = delete;
    Singleton<T>& operator=(const Singleton<T>&) = delete;
    Singleton<T>& operator=(Singleton<T>&&) = delete;
};
}

#endif
