#include "tcpv4_socket.hpp"

void TCPv4Socket::listen_on(const std::string &ip, uint16_t port)
{
    bind_to(ip, port);
    listen_on();
    register_fd(fd);
}

std::shared_ptr<TCPv4SocketClient> TCPv4Socket::connect_to(const std::string &ip, uint16_t port)
{
    auto client = std::make_shared<TCPv4SocketClient>();

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    ::inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

    if (::connect(client->fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::string error = std::string("[TCPv4SocketClient] Connect failed: ") + strerror(errno);
        throw SocketError(error);
    }

    return client;
}
