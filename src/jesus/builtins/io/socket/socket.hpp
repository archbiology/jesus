#pragma once

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <stdexcept>

class SocketError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

/**
 * @brief Base class for all sockets
 *
 * "For no one can lay any foundation other than the one already laid, which is Jesus Christ." — 1 Corinthians 3:11
 *
 * The expected child classes:
 *  -------------------------------------------
 *  | C++ class        | Meaning              |
 *  | ---------------- | -------------------- |
 *  | `TCPv4Socket`    | Reliable IPv4 stream |
 *  | `TCPv6Socket`    | Reliable IPv6 stream |
 *  | `UDPv4Socket`    | Datagram IPv4        |
 *  | `UDPv6Socket`    | Datagram IPv6        |
 *  | `ICMPv4Socket`   | Ping diagnostic      |
 *  | `EthernetSocket` | Frame capture        |
 *  -------------------------------------------
 */
class Socket
{
protected:
    int fd = -1;

public:
    Socket() = default;
    virtual ~Socket()
    {
        if (fd != -1)
            ::close(fd);
    }

    int get_fd() const { return fd; }

    virtual void send_data(const std::string &data)
    {
        ::send(fd, data.data(), data.size(), 0);
    }

    virtual std::string receive_data()
    {
        char buf[4096];
        ssize_t len = ::recv(fd, buf, sizeof(buf), 0);
        if (len <= 0)
            return {};

        return std::string(buf, len);
    }
};
