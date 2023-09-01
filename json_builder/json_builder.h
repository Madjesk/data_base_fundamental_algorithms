//
// Created by Клим Барабанов on 26.08.2023.
//

#ifndef KP_JSON_BUILDER_H
#define KP_JSON_BUILDER_H

#ifndef LOGGER_JSON_IMPLEMENTATION_H
#define LOGGER_JSON_IMPLEMENTATION_H
#include "../json/forJson/single_include/nlohmann/json.hpp"
#include "../logger/logger.h"

class json_builder
{

public:

    virtual logger *build(
            const std::string &path) = 0;

    virtual ~json_builder();

};

#endif //LOGGER_JSON_IMPLEMENTATION_H

#endif //KP_JSON_BUILDER_H
