#include "http_runtime.hpp"
#include "interpreter/interpreter.hpp"
#include "builtins/net/protocols/http/http_server.hpp"
#include "spirit/return_signal.hpp"

void HttpRuntime::addRoute(const HttpRoute &route)
{
    routes.push_back(std::move(route));
}

void HttpRuntime::serve()
{
    auto server = std::make_shared<HttpServer>();

    for (const auto &route : routes)
    {
        std::string key = "GET " + route.path;

        server->route(key, [this](const HttpRequest &req)
                      {
            HttpResponse res;

            res.body = handleRequest(req.path);

            res.headers["Content-Type"] = "text/plain";
            return res; });
    }

    int port = 7000;
    server->listen("0.0.0.0", port);

    std::cout << "🧠 HttpServer listening on " << port << "...\n";
    server->run();
}

HttpRoute *HttpRuntime::findRoute(const std::string &path)
{
    for (auto &route : routes)
    {
        if (route.path == path)
            return &route;
    }

    std::cerr << "ZERO routes registered.!!!\n";

    return nullptr;
}

std::string HttpRuntime::handleRequest(const std::string &path)
{
    auto *route = findRoute(path);

    if (!route)
    {
        std::cerr << "404 Not Found: " << path << std::endl;
        return "404 - Not Found";
    }

    std::cout << "Handling route: " << path << std::endl;

    try
    {
        for (auto stmt : route->body)
        {
            interpreter.execute(stmt);
        }
        return "";
    }
    catch (const ReturnSignal &ret)
    {
        return ret.value.toString();
    }
}
