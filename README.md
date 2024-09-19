# LOTEN + LOTUDP Protocol Stack - Performance Verification

This repository contains a comparative analysis and performance verification of the **LOTEN** (Low Overhead Transport via ESP-NOW) and **LOTUDP** (Low Overhead UDP) protocol stack, developed for secure, reliable, and low-latency communication in IoT systems. The protocols were designed to outperform existing communication technologies such as **BLE** (Bluetooth Low Energy) and **MQTT** (Message Queuing Telemetry Transport) in terms of efficiency and speed.

## Comparative Analysis: BLE + MQTT vs. LOTEN + LOTUDP

### Overview

We conducted a performance validation by comparing the message transmission times of **BLE + MQTT** versus our developed **LOTEN + LOTUDP** protocol stack. The test involved measuring the time taken for messages to travel from a **slave** device to the **master**, and then from the master to the **server**.

### BLE + MQTT Results

For BLE + MQTT communication, the time elapsed ranged between **1.688 to 1.831 seconds**. Below are the results from the test logs:

```plaintext
BLE + MQTT Time Elapsed:
1.7798819541931152 seconds
1.778791904449463 seconds
1.6884727478027344 seconds
1.7090754508972168 seconds
1.831474781036377 seconds
```
### LOTEN + LOTUDP Results
In comparison, the LOTEN + LOTUDP protocol stack exhibited near-zero response times, demonstrating its superior efficiency and speed. Additionally, the data transmitted using LOTEN passed through three slaves, further highlighting the protocol’s robustness in managing complex network topologies while maintaining low latency.

This performance advantage makes LOTEN + LOTUDP highly suitable for real-time applications, particularly in IoT environments where communication delays must be minimized.

## Key Advantages

+ Low Latency: The time for message transmission is significantly reduced compared to traditional BLE + MQTT setups.
Efficient Protocol Design: LOTEN and LOTUDP work in tandem to minimize overhead, ensuring fast and secure communication.
+ Robust in Complex Topologies: LOTEN can maintain minimal latency even when messages pass through multiple intermediary devices (slaves).
+ Real-Time Application: These protocols are ideal for scenarios requiring real-time communication, such as monitoring and control systems in IoT deployments.

## Conclusion

The LOTEN + LOTUDP protocol stack demonstrates a marked improvement over BLE + MQTT in terms of speed and efficiency, especially in scenarios involving multi-hop networks with multiple devices. This performance validation supports the use of LOTEN + LOTUDP for IoT systems that demand secure, reliable, and real-time communication.

