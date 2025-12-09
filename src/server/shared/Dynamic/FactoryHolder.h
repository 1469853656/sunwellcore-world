
#ifndef TRINITY_FACTORY_HOLDER
#define TRINITY_FACTORY_HOLDER

#include "Define.h"
#include "Dynamic/TypeList.h"
#include "ObjectRegistry.h"
#include "Singleton.h"

/** FactoryHolder holds a factory object of a specific type
 */
template<class T, class Key = std::string>
class FactoryHolder
{
public:
    typedef ObjectRegistry<FactoryHolder<T, Key>, Key> FactoryHolderRegistry;
    friend class Sunwell::Singleton<FactoryHolderRegistry>;
    typedef Sunwell::Singleton<FactoryHolderRegistry> FactoryHolderRepository;

    FactoryHolder(Key k)
        : i_key(k)
    {
    }
    virtual ~FactoryHolder() {}
    inline Key key() const { return i_key; }

    void RegisterSelf(void) { FactoryHolderRepository::instance()->InsertItem(this, i_key); }
    void DeregisterSelf(void) { FactoryHolderRepository::instance()->RemoveItem(this, false); }

    /// Abstract Factory create method
    virtual T* Create(void* data = nullptr) const = 0;

private:
    Key i_key;
};

/** Permissible is a classic way of letting the object decide
 * whether how good they handle things.  This is not retricted
 * to factory selectors.
 */
template<class T>
class Permissible
{
public:
    virtual ~Permissible() {}
    virtual int Permit(const T*) const = 0;
};
#endif
