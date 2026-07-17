#include "SocketCAN.hpp"
#include <iostream>
#include <iomanip>

int main() {
    // "can0" arayüzümüz için SocketCAN nesnesini oluşturuyoruz
    SocketCAN canBus("can0");

    if (!canBus.connect()) {
        return 1;
    }

    can_frame frame;
    std::cout << "STM32'den veri bekleniyor..." << std::endl;

    // Sonsuz döngüde gelen verileri ekrana basalım
    while (true) {
        if (canBus.receiveFrame(frame)) {
            // ID'yi hexadecimal formatta yazdır
            std::cout << "Alinan ID: 0x" << std::hex << frame.can_id 
                      << " [" << std::dec << static_cast<int>(frame.can_dlc) << "] Data: ";
            
            // Veri byte'larını ekrana bas
            for (int i = 0; i < frame.can_dlc; ++i) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                          << static_cast<int>(frame.data[i]) << " ";
            }
            std::cout << std::dec << std::endl; // Formatı normale döndür
        }
    }

    return 0;
}
