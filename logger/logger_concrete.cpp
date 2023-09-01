#include "logger_concrete.h"
#include "logger.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "date.h"

std::string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct = {};
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y/%m/%d  %X", &tstruct);
    return buf;
}

std::map<std::string, std::pair<std::ofstream *, size_t> > logger_concrete::_streams =
    std::map<std::string, std::pair<std::ofstream *, size_t> >();

logger_concrete::logger_concrete(
    std::map<std::string, logger::severity> const & targets)
{
    for (auto & target : targets)
    {
        auto global_stream = _streams.find(target.first);
        std::ofstream *stream = nullptr;

        if (global_stream == _streams.end())
        {
            if (target.first != "console") // TODO: ?!
            {
                stream = new std::ofstream;
                stream->open(target.first);
            }

            _streams.insert(std::make_pair(target.first, std::make_pair(stream, 1)));
        }
        else
        {
            stream = global_stream->second.first;
            global_stream->second.second++;
        }

        _logger_streams.insert(std::make_pair(target.first, std::make_pair(stream, target.second)));
    }
}


logger_concrete::logger_concrete(std::vector<std::pair<std::string, logger::severity>> const &streams)
{
    std::ofstream *stream = nullptr;

    for (int i = 0; i < streams.size(); i++)
    {
        auto global_stream_iter = _streams.find(streams[i].first);

        if (global_stream_iter == _streams.end())
        {
            if (streams[i].first != "console")
            {
                stream = new std::ofstream(streams[i].first, std::ios::out);
            }
            else
            {
                stream = nullptr;
            }

            _streams[streams[i].first] = make_pair(stream, 1);
        }
        else
        {
            stream = _streams[streams[i].first].first;
            _streams[streams[i].first].second++;
        }

        auto local_stream_iter = _logger_streams.find(streams[i].first);

        if (local_stream_iter == _logger_streams.end())
        {
            _logger_streams[streams[i].first] = make_pair(stream, streams[i].second);
        }
        else
        {
            local_stream_iter->second.second = streams[i].second;
        }
    }
}


logger_concrete::~logger_concrete()
{
    for (auto & logger_stream : _logger_streams)
    {
        auto global_stream = _streams.find(logger_stream.first);
        // TODO: check if global_stream == _streams.end()
        if (--(global_stream->second.second) == 0)
        {
            if (global_stream->second.first != nullptr)
            {
                global_stream->second.first->flush();
                global_stream->second.first->close();
                delete global_stream->second.first;
            }

            _streams.erase(global_stream);
        }
    }
}

logger const *logger_concrete::log(
    const std::string &to_log,
    logger::severity severity) const
{
    static std::string names[6] = {
            "TRACE",
            "DEBUG",
            "INFORMATION",
            "WARNING",
            "ERROR",
            "CRITICAL"
    };
    for (auto & logger_stream : _logger_streams)
    {
        if (logger_stream.second.second > severity)
        {
            continue;
        }

        if (logger_stream.second.first == nullptr)
        {
            std::cout << "[" << names[static_cast<int>(severity)] << "] " << "[" << currentDateTime() << "] " << to_log << std::endl;
        }
        else
        {
            (*logger_stream.second.first) << "[" << names[static_cast<int>(severity)] << "] " << "[" << currentDateTime() << "] " << to_log << std::endl;
        }
    }

    return this;
}
