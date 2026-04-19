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

private:
    std::vector<HttpRoute> routes;
    Interpreter &interpreter;
};
