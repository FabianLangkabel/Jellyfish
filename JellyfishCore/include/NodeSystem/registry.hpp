#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

template<typename Key>
class RegistryConstructionError : public std::exception
{
public:
    RegistryConstructionError(const Key &key)   :key(key){}

    const char *what()const throw(){return "Undefined key";}
    const Key key;
};

template<typename Base, typename... Args>
class Registry
{
public:
    typedef std::shared_ptr<Base> (*FactoryFunc)(Args... args);

    Registry() = default;
    ~Registry() = default;

    template<typename Derived>
    void registerClass(const std::string &key);
    void registerFactory(const std::string &key, const FactoryFunc factory);

    std::shared_ptr<Base> construct(const std::string &key, Args... args);
    std::vector<std::string> GetAllKeys()
    {
        std::vector<std::string> keys;
        keys.reserve(m_map.size());

        for(auto kv : m_map) {
            keys.push_back(kv.first);
        }
        return keys;
    }

private:
    template<typename Derived>
    static std::shared_ptr<Base> constructDerived(Args... args);

    std::unordered_map<std::string, FactoryFunc> m_map;

public:
    typedef RegistryConstructionError<std::string> ConstrError;
};

template<typename Base,
        typename... Args>
template<typename Derived>
void Registry<Base, Args...>::registerClass(const std::string &key)
{
    m_map[key] = &constructDerived<Derived>;
}

template<typename Base,
        typename... Args>
void Registry<Base, Args...>::registerFactory(const std::string &key, const FactoryFunc factory)
{
    m_map[key] = factory;
}

template<typename Base,
        typename... Args>
std::shared_ptr<Base> Registry<Base, Args...>::construct(const std::string &key, Args... args)
{
    try{
        return m_map.at(key)(std::forward<Args>(args)...);
    }
    catch(const std::out_of_range &e)
    {
        throw ConstrError(key);
    }
}

template<typename Base,
        typename... Args>
template<typename Derived>
std::shared_ptr<Base> Registry<Base, Args...>::constructDerived(Args... args)
{
    return std::make_shared<Derived>(std::forward<Args>(args)...);
}