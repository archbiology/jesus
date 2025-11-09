#pragma once

#include "../../../io/socket/tcpv4_socket.hpp"
#include "http_message.hpp"
#include <functional>
#include <unordered_map>
#include <iostream>
#include <memory>

/**
 * @brief High-level HTTP server built upon the TCPv4SocketServer foundation.
 *
 * The HttpServer class provides a simple yet efficient interface for handling
 * HTTP/1.1 requests over IPv4 sockets. It builds upon the event-driven
 * architecture provided by Mind (the central event loop) and the transport
 * abstraction defined by TCPv4SocketServer.
 *
 * This class allows defining routes, handling HTTP methods, and composing
 * dynamic responses through HttpRequest and HttpResponse objects.
 *
 * Example:
 * @code
 *  auto server = std::make_shared<HttpServer>();
    server->listen("0.0.0.0", 8080);

    server->route("GET /", [](const HttpRequest &req) {
        HttpResponse res;

        res.body = "<h1>Hello from the Mind!</h1>";
        res.headers["Content-Type"] = "text/html";
        res.headers["Date"] = gmt_time_rfc_9110().str();
        res.headers["Server"] = "SonOfMan (Jesus)";

        return res;
    });

    server->route("POST /echo", [](const HttpRequest &req) {

        HttpResponse res;

        res.body = "You said:\n" + req.body;
        res.headers["Content-Type"] = "text/plain";
        res.headers["Date"] = gmt_time_rfc_9110().str();
        res.headers["Server"] = "SonOfMan (Jesus)";

        return res;
    });

    server.run();

 * @endcode
 *
 * @note
 * The server runs in an event-driven loop powered by Mind, and supports
 * multiple concurrent connections through epoll (Linux).
 *
 * @see TCPv4SocketServer, HttpRequest, HttpResponse, Mind
 *
 * @author
 * Rúben Alves
 *
 * @remark
 * "For the Son of Man came not to be served but to serve." — Matthew 20:28
 */
class HttpServer : public TCPv4SocketServer
{
public:
    using Handler = std::function<HttpResponse(const HttpRequest &)>;

private:
    bool alreadyListening = false;
    std::unordered_map<std::string, Handler> routes;

public:
    bool listen(const std::string &addr, int port)
    {
        if (alreadyListening)
            return true;

        std::cout << "HttpServer.listen\n";

        listen_on(addr, port);
        alreadyListening = true;

        return true;
    }

    void route(const std::string &method_path, Handler handler)
    {
        routes[method_path] = handler;
    }

protected:
    void handle_event(int fd) override
    {
        if (fd == this->fd)
        {
            // -----------------
            // Accept new client
            // -----------------
            int client_fd = accept_client();
            register_fd(client_fd, EPOLLIN);
            return;
        }

        // ---------------
        // Receive request
        // ---------------
        std::string data = receive_from_fd(fd);
        if (data.empty())
        {
            unregister_fd(fd);
            ::close(fd);
            return;
        }

        // ------------------
        // Parse HTTP request
        // ------------------
        HttpRequest req = parse_request(data);

        // ------------
        // Find handler
        // ------------
        std::string key = req.method + " " + req.path;
        HttpResponse res;
        if (routes.contains(key))
            res = routes[key](req);
        else
        {
            res.status = 404;
            res.reason = "Not Found";
            res.body = "404 - Not Found";
        }

        // -------------
        // Send response
        // -------------
        std::string out = res.to_string();
        ::send(fd, out.data(), out.size(), 0);

        // -------------------------------------
        // Close after response (HTTP/1.0 style)
        // -------------------------------------
        unregister_fd(fd);
        ::close(fd);
    }

private:
    std::string receive_from_fd(int fd)
    {
        char buffer[4096];
        ssize_t len = ::recv(fd, buffer, sizeof(buffer), 0);
        if (len <= 0)
            return {};
        return std::string(buffer, len);
    }

    HttpRequest parse_request(const std::string &raw)
    {
        HttpRequest req;
        std::istringstream stream(raw);
        stream >> req.method >> req.path >> req.version;
        std::string line;
        std::getline(stream, line); // consume remainder of request line

        while (std::getline(stream, line) && line != "\r")
        {
            if (line.empty() || line == "\n")
                break;

            size_t colon = line.find(':');
            if (colon != std::string::npos)
            {
                std::string key = line.substr(0, colon);
                std::string value = line.substr(colon + 1);

                if (!value.empty() && value[0] == ' ')
                    value.erase(0, 1);

                if (!value.empty() && value.back() == '\r')
                    value.pop_back();

                req.headers[key] = value;
            }
        }

        // -------------------------
        // Remaining content as body
        // -------------------------
        std::string body;
        std::getline(stream, body, '\0');
        req.body = body;

        return req;
    }
};
