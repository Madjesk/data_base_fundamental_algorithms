#ifndef MY_COMPARATORS_H
#define MY_COMPARATORS_H
#include <iostream>
#include "../meeting_data/meeting_data.h"

class compare_str_keys
{
public:
    int operator()(const std::string &first_cmp, const std::string &second_cmp) const
    {
        return first_cmp.compare(second_cmp);
    }

};

class compare_data_keys
{
public:
    int operator()(const size_t &first_cmp, const size_t &second_cmp) const
    {
        return first_cmp - second_cmp;
    }
};

#endif