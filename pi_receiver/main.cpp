#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include "SocketCAN.h"
#include "CANParser.h"

std::atomic<bool> running(true);

// CAN hattını arka planda kesintisiz dinleyen thread
void receiveThreadFunc(SocketCAN* can) {
    can_frame frame;
    TelemetryData telemetry;

    while (running) {
        if (can->receiveFrame(frame)) {
            // Sadece STM32'den gelen telemetrileri çöz, ekrana bas
            if (frame.can_id == 0x103) {
                CANParser::parseTelemetry(frame, telemetry);
            }
        }
        // İşlemciyi sömürmesin diye çok küçük bir uyku (1ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main() {
    SocketCAN can("can0");

    if (!can.connect()) {
        std::cerr << "Soket baglantisi basarisiz oldu! can0 kapali olabilir." << std::endl;
        return 1;
    }

    std::cout << "can0 baglandi. Kontrol Paneli Aktif!" << std::endl;
    std::cout << "Komutlar: [1 + Enter] -> LED Yak | [0 + Enter] -> LED Kapat | [q + Enter] -> Cikis" << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;

    // Dinleme thread'ini ateşle
    std::thread rxThread(receiveThreadFunc, &can);

    std::string input;
    while (running) {
        std::cin >> input; // Karakter yerine string alarak buffer kaymalarını önlüyoruz

        if (input == "q" || input == "Q") {
            running = false;
            break;
        }

        can_frame txFrame;
        txFrame.can_id = 0x201; // STM32'nin beklediği komut ID'si
        txFrame.can_dlc = 1;    // 1 bayt veri

        if (input == "1") {
            txFrame.data[0] = 0x01; // LED_ON
            if (can.sendFrame(txFrame)) {
                std::cout << ">>> [PI -> STM32] KOMUT GÖNDERİLDİ: Turuncu/Mavi LED AÇ (0x01)" << std::endl;
            } else {
                std::cerr << "!!! Paket gönderim hatası!" << std::endl;
            }
        } 
        else if (input == "0") {
            txFrame.data[0] = 0x00; // LED_OFF
            if (can.sendFrame(txFrame)) {
                std::cout << ">>> [PI -> STM32] KOMUT GÖNDERİLDİ: Turuncu/Mavi LED KAPAT (0x00)" << std::endl;
            } else {
                std::cerr << "!!! Paket gönderim hatası!" << std::endl;
            }
        }
    }

    // Kapanış
    running = false;
    if (rxThread.joinable()) {
        rxThread.join();
    }
    can.disconnect();
    return 0;
}
