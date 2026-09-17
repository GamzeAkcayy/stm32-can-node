#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <cstring>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

std::atomic<bool> isRunning(true);

int openCanSocket(const char* ifname) {
    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Socket olusturulamadi");
        return -1;
    }

    struct ifreq ifr;
    std::strcpy(ifr.ifr_name, ifname);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        perror("SIOCGIFINDEX hatasi");
        close(s);
        return -1;
    }

    struct sockaddr_can addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind hatasi");
        close(s);
        return -1;
    }

    return s;
}

void receiverThread(int socketFd) {
    struct can_frame frame;
    while (isRunning) {
        int nbytes = read(socketFd, &frame, sizeof(struct can_frame));
        if (nbytes < 0) {
            if (!isRunning) break;
            perror("CAN Okuma Hatasi");
            break;
        }

        if (frame.can_id == 0x100 && frame.can_dlc >= 2) {
            uint8_t temp = frame.data[0];
            uint8_t counter = frame.data[1];
            std::cout << "\n[TELEMETRY RX] ID: 0x100 | Temp: " 
                      << static_cast<int>(temp) << " C | Counter: " 
                      << static_cast<int>(counter) << std::endl;
            std::cout << "Komut girin ('t' = Toggle LED, 'q' = Cikis): " << std::flush;
        }
    }
}

void senderThread(int socketFd) {
    std::string input;
    std::cout << "Komut girin ('t' = Toggle LED, 'q' = Cikis): " << std::flush;
    
    while (isRunning && std::cin >> input) {
        if (input == "t" || input == "1") {
            struct can_frame txFrame;
            txFrame.can_id = 0x200;
            txFrame.can_dlc = 8;
            std::memset(txFrame.data, 0, 8);
            txFrame.data[0] = 0x01;

            int nbytes = write(socketFd, &txFrame, sizeof(struct can_frame));
            if (nbytes != sizeof(struct can_frame)) {
                std::cerr << "\n[HATA] Komut cercevesi gonderilemedi!" << std::endl;
            } else {
                std::cout << "\n[COMMAND TX] 0x200 gonderildi -> STM32 LED Toggle tetiklendi." << std::endl;
            }
        } else if (input == "q") {
            isRunning = false;
            break;
        } else {
            std::cout << "\nBilinmeyen komut. 't' (Toggle) veya 'q' (Cikis) kullanin." << std::endl;
        }
        std::cout << "Komut girin ('t' = Toggle LED, 'q' = Cikis): " << std::flush;
    }
}

int main() {
    const char* can_interface = "can0";
    int socketFd = openCanSocket(can_interface);
    if (socketFd < 0) {
        return 1;
    }

    std::cout << "=== STM32 - Raspberry Pi CAN Kontrol Paneli Baslatildi ===" << std::endl;

    std::thread rxWorker(receiverThread, socketFd);
    std::thread txWorker(senderThread, socketFd);

    txWorker.join();
    isRunning = false;
    close(socketFd);
    if (rxWorker.joinable()) {
        rxWorker.join();
    }

    std::cout << "Program sonlandirildi." << std::endl;
    return 0;
}
