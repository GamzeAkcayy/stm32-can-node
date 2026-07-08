# STM32 CAN Bus Real-Time Analyzer & Hardware-Level Filter

A high-performance embedded systems project demonstrating real-time Controller Area Network (CAN) communication, bare-metal peripheral configuration, and hardware-level message filtering on STM32 microcontrollers.

## 📌 Project Motivation & Objectives
In resource-constrained edge environments, processing every single CAN frame via software interrupts causes massive CPU overhead and high latency. This project solves that efficiency bottleneck by leveraging the STM32's onboard hardware acceptance filters to drop irrelevant traffic before it ever hits the CPU, ensuring deterministic, real-time data analysis for critical payloads.

- **Zero Software Overhead Filtering:** Configured 16/32-bit scale mask/identifier filters to segment critical data.
- **Real-Time Diagnostics:** Formatted telemetry streams to analyze bus status and capture error frames (Form, Stuff, Acknowledgment errors).
- **Robust Hardware Integration:** Debugged and optimized transceiver loopback and normal operation states under high bus load.

## 🛠️ Hardware Stack
- **MCU:** STM32F407VGTX (Discovery Board / Custom Board)
- **CAN Transceiver:** SN65HVD230 Module
- **Analysis Tool:** Logic Analyzer / STM32CubeIDE Live Watch

## 📐 System Architecture
*(Hardware block diagram and data flow model will be embedded here upon architectural review.)*

## 🔧 Pinout Configuration

| Component | STM32 Pin | Transceiver Pin | Description |
|---|---|---|---|
| SN65HVD230 | PA11 | RX | CAN Receive |
| SN65HVD230 | PA12 | TX | CAN Transmit |
| SN65HVD230 | GND | GND | Common Ground (Critical for signal integrity) |
| SN65HVD230 | 3.3V | VCC | Power Supply |

## 📈 Current Status & Roadmap
- [x] Hardware layer debugging & Ground loop short-circuit resolution.
- [ ] Basic CAN TX/RX node communication loops.
- [ ] Hardware filter implementation (Mask & List modes).
- [ ] Error frame logging and latency stress test.
- [ ] Comprehensive documentation.