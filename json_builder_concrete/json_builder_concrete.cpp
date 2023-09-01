#include "json_builder_concrete.h"

logger* json_builder_concrete::build(
        const std::string &path)
{
    std::map<std::string, logger::severity> severity_str = {
            {"TRACE", logger::severity::trace},
            {"DEBUG", logger::severity::debug},
            {"INFORMATION", logger::severity::information},
            {"WARNING", logger::severity::warning},
            {"ERROR", logger::severity::error},
            {"CRITICAL", logger::severity::critical}
    };

    _streams.clear();

    std::map<std::string, logger::severity> _configuration;
    nlohmann::json obj_json;
    std::fstream json_file;

    json_file.open(path);
    json_file >> obj_json;
    json_file.close();

    for (int i = 0; i < obj_json["configs"].size(); i++)
    {
        _configuration[obj_json["configs"][i]["target"]] = severity_str[obj_json["configs"][i]["severity"]];
    }

    for (auto &item: _configuration)
    {
        _streams.emplace_back(item.first, item.second);
    }

    return new logger_concrete(_streams);
}