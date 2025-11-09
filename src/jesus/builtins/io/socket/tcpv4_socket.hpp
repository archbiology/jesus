#pragma once

#include "../../../spirit/mind.hpp"
#include "socket.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <memory>

class TCPv4SocketServer; // Forward declaration
class TCPv4SocketClient; // Forward declaration

class TCPv4Socket : public Socket, public Mind
{
protected:
    sockaddr_in addr{};

public:
    TCPv4Socket()
    {
        std::cout << "TCPv4Socket() constructor called.\n";
        fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
            throw SocketError(errnoMsg("Failed to create socket"));
    }

    void listen_on(const std::string &ip, uint16_t port);

    static std::shared_ptr<TCPv4SocketClient> connect_to(const std::string &ip, uint16_t port);

    void bind_to(const std::string &ip, uint16_t port)
    {
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (::inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0)
            throw SocketError(errnoMsg("Invalid IPv4 address"));

        if (::bind(fd, (sockaddr *)&addr, sizeof(addr)) < 0)
            throw SocketError(errnoMsg("Bind failed"));
    }

    void listen_on(int backlog = 10)
    {
        if (::listen(fd, backlog) < 0)
            throw SocketError(errnoMsg("Listen failed"));
    }

    int accept_client()
    {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_fd = ::accept(fd, (sockaddr *)&client_addr, &len);
        if (client_fd < 0)
            throw SocketError(errnoMsg("Accept failed"));

        return client_fd;
    }

protected:
    void handle_event(int fd) override
    {
        if (fd == this->fd)
        {
            int client_fd = accept_client();
            register_fd(client_fd, EPOLLIN);
            std::cout << "[TCPv4Socket] Accepted new client: " << client_fd << "\n";
        }
        else
        {
            char buffer[1024];
            ssize_t len = ::recv(fd, buffer, sizeof(buffer), 0);
            if (len <= 0)
            {
                unregister_fd(fd);
                ::close(fd);
                std::cout << "[TCPv4Socket] Client disconnected: " << fd << "\n";
            }
            else
            {
                std::string data(buffer, len);
                std::cout << "[TCPv4Socket] Received: " << data << "\n";
                ::send(fd, data.data(), data.size(), 0); // echo back
            }
        }
    }
};

class TCPv4SocketClient : public TCPv4Socket
{
};

class TCPv4SocketServer : public TCPv4Socket
{
};
