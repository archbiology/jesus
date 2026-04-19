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

        server->route(key, [this, route](const HttpRequest &req) {
            HttpResponse res;

            try
            {
                for (auto stmt : route.body)
                    interpreter.execute(stmt);

                res.body = "";
            }
            catch (const ReturnSignal &ret)
            {
                res.body = ret.value.toString();
            }

            res.headers["Content-Type"] = route.contentType;

            return res;
        });
    }

    int port = 7000;
    server->listen("0.0.0.0", port);

    std::cout << "🧠 HttpServer listening on " << port << "...\n";
    server->run();
}
