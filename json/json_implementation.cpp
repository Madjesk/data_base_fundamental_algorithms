#include "json_implementation.h"

json_implementation::json_implementation(const std::string &path)
{
    nlohmann::json obj_json;
    std::fstream json_file;

    json_file.open(path);
    json_file >> obj_json;
    json_file.close();

    for (int i = 0; i < obj_json["configs"].size(); i++)
    {
        _configuration[obj_json["configs"][i]["target"]] = obj_json["configs"][i]["severity"];
    }
}

logger* json_implementation::create() const
{
    return new logger_implementation(_configuration);
}
