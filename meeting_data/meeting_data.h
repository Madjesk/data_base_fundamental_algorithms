#ifndef MEETING_DATA_H
#define MEETING_DATA_H

#include "../flyweight/flyweight_for_string.h"

class meeting_data
{

private:

    std::shared_ptr<flyweight_for_string> _id;
    std::shared_ptr<flyweight_for_string> _type;
    std::shared_ptr<flyweight_for_string> _format;
    std::shared_ptr<flyweight_for_string> _description;
    std::shared_ptr<flyweight_for_string> _link;
    std::shared_ptr<flyweight_for_string> _surname;
    std::shared_ptr<flyweight_for_string> _name;
    std::shared_ptr<flyweight_for_string> _patronymic;
    std::shared_ptr<flyweight_for_string> _date;
    std::shared_ptr<flyweight_for_string> _start_time;
    std::shared_ptr<flyweight_for_string> _duration;
    std::shared_ptr<flyweight_for_string> _members;

public:
    std::shared_ptr<flyweight_for_string> get_id() const;
    void set_id(const std::string &description);

    std::shared_ptr<flyweight_for_string> get_type() const;
    void set_type(const std::string &description);

    std::shared_ptr<flyweight_for_string> get_format() const;
    void set_format(const std::string &description);

    std::shared_ptr<flyweight_for_string> get_description() const;
    void set_description(const std::string &description);

    std::shared_ptr<flyweight_for_string> get_link() const;
    void set_link(const std::string &description);

    std::shared_ptr<flyweight_for_string> get_surname() const;
    void set_surname(const std::string &description);

    std::shared_ptr<flyweight_for_string> get_name() const;
    void set_name(const std::string &description);

    std::shared_ptr<flyweight_for_string> get_patronymic() const;
    void set_patronymic(const std::string &patronymic);

    std::shared_ptr<flyweight_for_string> get_date() const;
    void set_date(const std::string &description);

    std::shared_ptr<flyweight_for_string> get_start_time() const;
    void set_start_time(const std::string &description);

    std::shared_ptr<flyweight_for_string> get_duration() const;
    void set_duration(const std::string &description);

    std::shared_ptr<flyweight_for_string> get_members() const;
    void set_members(const std::string &description);

};

#endif