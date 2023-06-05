#include "logger_concrete.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>


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
            if (target.first != "console")
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

logger_concrete::~logger_concrete()
{
    for (auto & logger_stream : _logger_streams)
    {
        auto global_stream = _streams.find(logger_stream.first);
        if (global_stream == _streams.end()){
            break;
        }
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

logger const *logger_concrete::log(const std::string &to_log, logger::severity severity) const
{
    std::vector<std::string> severity_mode = {"TRACE", "DEBUG", "INFORMATION",
    "WARNING", "ERROR", "CRITICAL"};

    for (auto & logger_stream : _logger_streams)
    {
        if (logger_stream.second.second > severity)
        {
            continue;
        }
        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);

        if (logger_stream.second.first == nullptr)
        {
            std::cout << "[" 
            << severity_mode.at(static_cast<int>(severity)) << "]" 
            << "[" << (now->tm_mday) << "/" << (now->tm_mon + 1) << '/' 
            << (now->tm_year + 1900) << ' '
            << (now->tm_hour) << ":" << (now->tm_min) << ":" << (now->tm_sec) << "] " 
            << to_log << std::endl;
        }
        else
        {
            (*logger_stream.second.first) << "[" 
            << severity_mode.at(static_cast<int>(severity)) << "]" 
            << "[" << (now->tm_mday) << "/" << (now->tm_mon + 1) << '/' 
            << (now->tm_year + 1900) << ' '
            << (now->tm_hour) << ":" << (now->tm_min) << ":" << (now->tm_sec) << "] " 
            << to_log << std::endl;
        }
    }

    return this;
}
