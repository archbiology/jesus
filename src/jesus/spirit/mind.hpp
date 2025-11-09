#pragma once

#include <sys/epoll.h> // For epoll_create1, epoll_ctl, epoll_wait
#include <unistd.h>    // For close(), read(), write(), etc.
#include <vector>
#include <stdexcept>
#include <cerrno>    // For errno
#include <cstring>   // For strerror
#include <algorithm> // for std::remove

class UnstableMindError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

/**
 * @brief The central Event Loop/Bus, where all events are received and dispatched.
 *
 * "Who has known the mind of the Lord so as to instruct him?" But we have the mind of Christ. — 1 Corinthians 2:16
 */
class Mind
{
protected:
    int epoll_fd;
    std::vector<int> fds;

    virtual void handle_event(int fd) = 0; // implement per-protocol

public:
    Mind()
    {
        epoll_fd = ::epoll_create1(0);
        if (epoll_fd == -1)
            throw UnstableMindError(errnoMsg("Failed to create epoll instance"));
    }

    virtual ~Mind()
    {
        if (epoll_fd != -1)
            ::close(epoll_fd);
    }

    std::string errnoMsg(std::string prefix)
    {
        return prefix + ": " + strerror(errno);
    }

    void register_fd(int fd, uint32_t events = EPOLLIN)
    {
        epoll_event ev{};
        ev.events = events;
        ev.data.fd = fd;
        if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
            throw UnstableMindError(errnoMsg("Failed to add fd to epoll"));

        fds.push_back(fd);
    }

    void unregister_fd(int fd)
    {
        ::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
        fds.erase(std::remove(fds.begin(), fds.end(), fd), fds.end());
    }

    void run()
    {
        constexpr int MAX_EVENTS = 64;
        epoll_event events[MAX_EVENTS];

        while (true)
        {
            int n = ::epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
            if (n == -1)
            {
                if (errno == EINTR)
                    continue;

                throw UnstableMindError(errnoMsg("epoll_wait failed"));
            }

            for (int i = 0; i < n; ++i)
            {
                handle_event(events[i].data.fd);
            }
        }
    }
};
