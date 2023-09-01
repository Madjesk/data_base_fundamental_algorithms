#include "flyweight_for_string.h"

void flyweight_for_string::set_value(const std::string &value)
{
    _value = value;
}

const std::string &flyweight_for_string::get_value() const{
    return _value;
}

flyweight_factory_for_string &flyweight_factory_for_string::get_instance()
{
    static flyweight_factory_for_string instance;
    return instance;
}

std::shared_ptr<flyweight_for_string> flyweight_factory_for_string::get_string_flyweight(const std::string &value)
{
    auto element = _hashmap_strings_flyweights.find(value);
    if(element != _hashmap_strings_flyweights.end())
    {
        return element->second;
    }
    auto flyweight = std::make_shared<flyweight_for_string>();
    flyweight->set_value(value);
    _hashmap_strings_flyweights[value] = flyweight;

    return flyweight;
}
