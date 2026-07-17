#ifndef SOCKETCAN_HPP
#define SOCKETCAN_HPP

#include <string>
#include <linux/can.h>

class SocketCAN {
public:
    explicit SocketCAN(const std::string& interfaceName);
    ~SocketCAN();

    // Kopyalamayı engelleyerek kaynak yönetimini güvenli tutuyoruz (RAII)
    SocketCAN(const SocketCAN&) = delete;
    SocketCAN& operator=(const SocketCAN&) = delete;

    bool connect();
    void disconnect();
    bool receiveFrame(can_frame& frame);
    bool sendFrame(const can_frame& frame);

private:
    std::string m_interfaceName;
    int m_socketFd;
};

#endif // SOCKETCAN_HPP
