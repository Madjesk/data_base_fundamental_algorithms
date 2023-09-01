#include "meeting_data.h"


std::shared_ptr<flyweight_for_string> meeting_data::get_id() const
{
    return _id;
}

void meeting_data::set_id(const std::string &id)
{
    flyweight_factory_for_string &factory = flyweight_factory_for_string::get_instance();
    _id = factory.get_string_flyweight(id);
}

std::shared_ptr<flyweight_for_string> meeting_data::get_type() const
{
    return _type;
}

void meeting_data::set_type(const std::string &type)
{
    flyweight_factory_for_string &factory = flyweight_factory_for_string::get_instance();
    _type = factory.get_string_flyweight(type);
}

std::shared_ptr<flyweight_for_string> meeting_data::get_format() const
{
    return _format;
}

void meeting_data::set_format(const std::string &format)
{
    flyweight_factory_for_string &factory = flyweight_factory_for_string::get_instance();
    _format = factory.get_string_flyweight(format);
}

std::shared_ptr<flyweight_for_string> meeting_data::get_description() const
{
    return _description;
}

void meeting_data::set_description(const std::string &description)
{
    flyweight_factory_for_string &factory = flyweight_factory_for_string::get_instance();
    _description = factory.get_string_flyweight(description);
}

std::shared_ptr<flyweight_for_string> meeting_data::get_link() const
{
    return _link;
}

void meeting_data::set_link(const std::string &link)
{
    flyweight_factory_for_string &factory = flyweight_factory_for_string::get_instance();
    _link = factory.get_string_flyweight(link);
}

std::shared_ptr<flyweight_for_string> meeting_data::get_surname() const
{
    return _surname;
}

void meeting_data::set_surname(const std::string &surname)
{
    flyweight_factory_for_string &factory = flyweight_factory_for_string::get_instance();
    _surname = factory.get_string_flyweight(surname);
}


std::shared_ptr<flyweight_for_string> meeting_data::get_name() const
{
    return _name;
}

void meeting_data::set_name(const std::string &name)
{
    flyweight_factory_for_string &factory = flyweight_factory_for_string::get_instance();
    _name = factory.get_string_flyweight(name);
}


std::shared_ptr<flyweight_for_string> meeting_data::get_patronymic() const
{
    return _patronymic;
}

void meeting_data::set_patronymic(const std::string &patronymic)
{
    flyweight_factory_for_string &factory = flyweight_factory_for_string::get_instance();
    _patronymic = factory.get_string_flyweight(patronymic);
}

std::shared_ptr<flyweight_for_string> meeting_data::get_date() const
{
    return _date;
}

void meeting_data::set_date(const std::string &date)
{
    flyweight_factory_for_string &factory = flyweight_factory_for_string::get_instance();
    _date = factory.get_string_flyweight(date);
}

std::shared_ptr<flyweight_for_string> meeting_data::get_start_time() const
{
    return _start_time;
}

void meeting_data::set_start_time(const std::string &start_time)
{
    flyweight_factory_for_string &factory = flyweight_factory_for_string::get_instance();
    _start_time = factory.get_string_flyweight(start_time);
}

std::shared_ptr<flyweight_for_string> meeting_data::get_duration() const
{
    return _duration;
}

void meeting_data::set_duration(const std::string &duration)
{
    flyweight_factory_for_string &factory = flyweight_factory_for_string::get_instance();
    _duration = factory.get_string_flyweight(duration);
}

std::shared_ptr<flyweight_for_string> meeting_data::get_members() const
{
    return _members;
}

void meeting_data::set_members(const std::string &members)
{
    flyweight_factory_for_string &factory = flyweight_factory_for_string::get_instance();
    _members = factory.get_string_flyweight(members);
}

