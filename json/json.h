#ifndef JSON_H
#define JSON_H
#include "forJson/single_include/nlohmann/json.hpp"
#include "logger.h"

class json
{
public:
    virtual logger* create() const = 0;

    virtual ~json() {};
};

#endif //JSON_H
