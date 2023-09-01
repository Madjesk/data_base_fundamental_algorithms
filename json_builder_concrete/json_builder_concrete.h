////
//// Created by Клим Барабанов on 26.08.2023.
////
//
#ifndef KP_JSON_BUILDER_CONCRETE_H
#define KP_JSON_BUILDER_CONCRETE_H

#include <fstream>
#include <vector>
#include "logger.h"
#include "logger_concrete.h"
#include "../json_builder/json_builder.h"
#include "../logger/logger_concrete.h"

class json_builder_concrete final : public json_builder
{

private:

    std::vector<std::pair<std::string, logger::severity>> _streams;

public:

    logger *build(
            const std::string &path) override;

};

#endif //KP_JSON_BUILDER_CONCRETE_H
