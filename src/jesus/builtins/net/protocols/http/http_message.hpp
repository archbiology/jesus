#pragma once
#include <string>
#include <unordered_map>
#include <sstream>

struct HttpRequest
{
    std::string method;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

struct HttpResponse
{
    int status = 200;
    std::string reason = "OK";
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    std::string to_string() const
    {
        std::ostringstream oss;
        oss << "HTTP/1.1 " << status << " " << reason << "\r\n";

        for (auto &h : headers)
            oss << h.first << ": " << h.second << "\r\n";

        oss << "Content-Length: " << body.size() << "\r\n\r\n";
        oss << body;
        return oss.str();
    }
};
