#pragma once
#include <vector>
#include "http_route.hpp"

class Interpreter;

class HttpRuntime
{

public:
    explicit HttpRuntime(Interpreter &interpreter) : interpreter(interpreter) {}

    void addRoute(const HttpRoute &route);
    void serve();

    HttpRoute *findRoute(const std::string &path);
    std::string handleRequest(const std::string &path);

private:
    std::vector<HttpRoute> routes;
    Interpreter &interpreter;
};
