#include "SocketCAN.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

SocketCAN::SocketCAN(const std::string& interfaceName)
    : m_interfaceName(interfaceName), m_socketFd(-1) {}

SocketCAN::~SocketCAN() {
    disconnect();
}

bool SocketCAN::connect() {
    // 1. Ham CAN Soketi Oluştur
    m_socketFd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (m_socketFd < 0) {
        std::cerr << "Hata: CAN soketi acilamadi!" << std::endl;
        return false;
    }

    // 2. Arayüz ismini (can0) indeks numarasına çevir
    struct ifreq ifr;
    std::strncpy(ifr.ifr_name, m_interfaceName.c_str(), IFNAMSIZ - 1);
    if (ioctl(m_socketFd, SIOCGIFINDEX, &ifr) < 0) {
        std::cerr << "Hata: Arayuz bulunamadi: " << m_interfaceName << std::endl;
        close(m_socketFd);
        m_socketFd = -1;
        return false;
    }

    // 3. Soketi CAN arayüzüne bağla (Bind)
    struct sockaddr_can addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(m_socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Hata: Soket baglanamadi (Bind hatasi)!" << std::endl;
        close(m_socketFd);
        m_socketFd = -1;
        return false;
    }

    std::cout << m_interfaceName << " basariyla baglandi. Dinleniyor..." << std::endl;
    return true;
}

void SocketCAN::disconnect() {
    if (m_socketFd >= 0) {
        close(m_socketFd);
        m_socketFd = -1;
        std::cout << "Soket kapatildi." << std::endl;
    }
}

bool SocketCAN::receiveFrame(can_frame& frame) {
    if (m_socketFd < 0) return false;

    // Engellemeli (blocking) okuma yapar; veri gelene kadar burada bekler
    ssize_t nBytes = read(m_socketFd, &frame, sizeof(struct can_frame));
    if (nBytes < 0) {
        std::cerr << "Hata: Veri okunamadi!" << std::endl;
        return false;
    }

    if (nBytes < static_cast<ssize_t>(sizeof(struct can_frame))) {
        std::cerr << "Hata: Eksik CAN paketi alindi!" << std::endl;
        return false;
    }

    return true;
}

bool SocketCAN::sendFrame(const can_frame& frame) {
    if (m_socketFd < 0) return false;

    ssize_t nBytes = write(m_socketFd, &frame, sizeof(struct can_frame));
    return (nBytes == sizeof(struct can_frame));
}
