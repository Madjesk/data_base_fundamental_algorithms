#ifndef FLYWEIGHT_FOR_STRING_H
#define FLYWEIGHT_FOR_STRING_H

#include <iostream>
#include <unordered_map>
#include <memory>

class flyweight_for_string
{
private:
    std::string _value;
public:
    void set_value(const std::string &value);
    const std::string &get_value() const;
};

class flyweight_factory_for_string
{
private:
    std::unordered_map<std::string, std::shared_ptr<flyweight_for_string>> _hashmap_strings_flyweights;
public:
    static flyweight_factory_for_string &get_instance();
    std::shared_ptr<flyweight_for_string>(get_string_flyweight(const std::string &value));
};

#endif
