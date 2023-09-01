#ifndef JSON_IMPLEMENTATION_H
#define JSON_IMPLEMENTATION_H
#include <fstream>
#include "logger_implementation.h"
#include "json.h"

class json_implementation final : public json
{
private:
    std::map<std::string, logger::severity> _configuration;

public:
    json_implementation(const std::string &path);

    logger* create() const override;
};

#endif //JSON_IMPLEMENTATION_H
