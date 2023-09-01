#include "logger_builder_concrete.h"
#include "logger_concrete.h"
#include "logger.h"
#include <fstream>
#include <map>

logger_builder *logger_builder_concrete::add_stream(
    std::string const &path,
    logger::severity severity)
{
    _construction_info[path] = severity;

    return this;
}

logger *logger_builder_concrete::construct() const
{
    return new logger_concrete(_construction_info);
}

logger_builder *logger_builder_concrete::build(std::string const &path)
{
    std::string file_path, type;
    std::fstream file(path);
    std::map<std::string, logger::severity> severity_types = {
            {"trace", logger::severity::trace},
            {"debug", logger::severity::debug},
            {"information", logger::severity::information},
            {"warning", logger::severity::warning},
            {"error", logger::severity::error},
            {"critical", logger::severity::critical},
    };
    //try catch
    if(!file.is_open()) {
        return nullptr;
    }


    while (file >> file_path >> type) {
        add_stream(file_path, severity_types[type]);
    }
    return this;
}