# StreamForge

**High-throughput SPI data pipeline for the ESP32-S3, built around DMA, double buffering, FreeRTOS queues, and explicit CPU/DMA buffer ownership.**

StreamForge is an embedded systems project focused on building and validating a reliable SPI data pipeline rather than simply transferring bytes between two devices.

The project uses an ESP32-S3 as the master and an SPI slave, with DMA-backed transactions and a double-buffered producer/consumer architecture designed to keep data moving while the CPU processes completed buffers.

---

## Overview

The core problem StreamForge explores is:

> **How can an embedded system continuously receive SPI data through DMA while the CPU processes previous data without corrupting buffers or stalling the pipeline?**

Instead of having the CPU wait for every SPI transaction to finish, StreamForge separates **DMA ownership** from **CPU ownership**.

```text
                    SPI
                     │
                     ▼
              ┌─────────────┐
              │  SPI / DMA  │
              └──────┬──────┘
                     │
              Completed Buffer
                     │
                     ▼
              ┌─────────────┐
              │  Full Queue │
              └──────┬──────┘
                     │
                     ▼
              ┌─────────────┐
              │     CPU     │
              │  Consumer   │
              └──────┬──────┘
                     │
                Buffer Cleared
                     │
                     ▼
              ┌─────────────┐
              │ Empty Queue │
              └──────┬──────┘
                     │
                     ▼
                 DMA Reuse

```
The result is a small producer/consumer pipeline where buffers continuously circulate between the DMA subsystem and the CPU.

## Key Features
- ESP32-S3 SPI master/slave communication
- ESP-IDF
- FreeRTOS
- SPI DMA transactions
- Double buffering
- Producer/consumer architecture
- FreeRTOS queues for buffer ownership transfer
- Explicit CPU/DMA buffer ownership
- Sequence-number validation
- Magic-byte packet validation
- Malformed-frame detection
- Buffer recycling
- DMA-capable heap allocation
- Stress testing
- Packet, sequence-error, malformed-frame, and timeout statistics
- Memory-pressure testing
- Configurable transaction sizes

---
## Architecture

StreamForge uses two buffers as a rotating pool.

At any given time, a buffer is either:

Available for DMA
Owned by the DMA pipeline
Waiting for CPU processing
Owned by the CPU
Returned to the DMA pool

The queues represent ownership rather than simply acting as notification mechanisms.

Empty Queue

Contains buffers available for another DMA transaction.

```
empty_queue
    │
    ├── Buffer 0
    └── Buffer 1
```
Full Queue

Contains buffers whose DMA transaction has completed and are ready for CPU processing.
```
full_queue
    │
    ├── Buffer 0
    └── Buffer 1
```

The consumer retrieves a completed buffer, validates and clears it, then returns it to the empty queue.

This prevents the CPU from modifying a buffer while DMA may still be using it.

---

## Packet Format

StreamForge uses a simple packet structure for validation.
```
┌────────────┬────────────┬────────────────────────┐
│ Magic Byte │ Sequence # │       Payload          │
└────────────┴────────────┴────────────────────────┘
     1 B          1 B              N B
```
###Magic Byte

The first byte identifies a valid StreamForge frame.

If the expected magic byte is missing, the frame is treated as malformed and discarded.

Sequence Number

The second byte is incremented for every transmitted packet.

The consumer maintains an expected sequence number and compares it against the received value.

This allows StreamForge to detect:

- Dropped frames
- Unexpected ordering
- Synchronization problems
- Pipeline failures

When a sequence break is detected, the consumer resynchronizes to the received sequence number.

---

## Buffer Ownership

One of the primary design goals is making ownership explicit.

              ┌─────────────┐
              │ Empty Queue │
              └──────┬──────┘
                     │
                     ▼
                   DMA
                     │
                     ▼
              ┌─────────────┐
              │ Full Queue  │
              └──────┬──────┘
                     │
                     ▼
                   CPU
                     │
              clear / validate
                     │
                     ▼
              ┌─────────────┐
              │ Empty Queue │
              └─────────────┘

The CPU does not process a buffer until it has been transferred to the full queue.

Likewise, a buffer is not returned to the DMA pipeline until CPU processing has completed.

This makes buffer reuse deterministic and avoids accidental concurrent access.

---

## DMA

StreamForge allocates its transaction buffers from memory suitable for DMA operations.

The allocation layer handles creation of the transmit and receive buffer pools and validates allocation success before the pipeline begins.

The goal is to keep the DMA layer independent from the higher-level consumer logic.

This also makes it possible to experiment with different buffer sizes and transaction counts without rewriting the pipeline architecture.

---

## Concurrency

The system uses separate FreeRTOS tasks for different responsibilities.

Conceptually:
```
Master / Producer
        │
        ▼
     SPI DMA
        │
        ▼
     Full Queue
        │
        ▼
    Consumer
        │
        ▼
   Empty Queue
```

The producer and consumer operate independently and communicate through FreeRTOS queues.

This allows the CPU to process one completed transaction while another buffer is being prepared for DMA.

---
## Validation

Each received frame goes through several checks.

Sequence Validation
`received_seq == expected_seq`

If the values differ, a sequence error is recorded and the consumer resynchronizes.

---

### Packet Validation

The magic byte is checked independently.

A malformed packet is dropped rather than being passed further through the pipeline.

---

### Statistics

The stress-test consumer tracks:
```
Received packets
Valid packets
Sequence errors
Malformed packets
Task timeouts
```
Example:
```
Received packets: 61
Valid packets: 61
Sequence break packets: 0
Malformed packets: 0
Task timeouts: 0
Stress Testing
```
StreamForge includes a stress-testing path intended to expose pipeline failures under continuous operation.

The test tracks both functional correctness and resource behavior.

Example metrics include:

packet throughput
packet validity
sequence integrity
malformed frames
task timeouts
DMA-capable heap before/after testing

Memory pressure can be measured using the DMA-capable heap:

DMA heap before: 303240
DMA heap after:  290308
DMA heap delta:  -12932

This provides a simple way to observe the memory cost of the active DMA pipeline.

The stress-test logic is primarily a development/validation tool and is separate from the core data-transfer architecture.

---

Why StreamForge?

StreamForge was built to explore a problem that is easy to hide behind high-level APIs:

`How do you move data continuously through an embedded system while multiple components need access to the same memory at different times?`

The project focuses on the underlying mechanics:

- DMA
- memory ownership
- buffering
- synchronization
- queue-based communication
- validation
- resource constraints

rather than simply demonstrating that SPI communication works.

---

### Technology

| Area            | Technology                  |
| --------------- | --------------------------- |
| MCU             | ESP32-S3                    |
| Language        | C                           |
| Framework       | ESP-IDF                     |
| RTOS            | FreeRTOS                    |
| Communication   | SPI                         |
| Data Transfer   | DMA                         |
| Synchronization | FreeRTOS Queues             |
| Memory          | DMA-capable heap            |
| Testing         | Stress / validation testing |

---

## Project Structure

A simplified view of the project:
```
StreamForge/
├── spi_master/
│   ├── main/
│   │   ├── master.c
│   │   ├── ...
│   │   └── ...
│   └── ...
│
├── spi_slave/
│   ├── main/
│   │   ├── slave.c
│   │   ├── ...
│   │   └── ...
│   └── ...
│
└── README.md
```
The exact structure may evolve as additional testing and infrastructure are added.

---

## Roadmap

StreamForge is intentionally versioned as an evolving embedded systems project.

**v1.0.0**

Core SPI/DMA pipeline:

- SPI communication
- DMA
- double buffering
- FreeRTOS queues
- CPU/DMA ownership
- packet validation
- sequence tracking
- stress testing

**v1.1.0**

- Data-integrity improvements:
- CRC-based packet validation
- additional edge-case handling

**v1.2.0**

- Testing infrastructure:
- Unity test framework
- CMock
- expanded test coverage

**v1.3.0**

- Higher-throughput SPI architecture:
- Quad SPI
- larger transactions
- continued buffer/throughput optimization

The project is intended to stop once the architecture has served its purpose rather than endlessly adding features.

---

## Future Possibilities

Potential extensions include:

- larger transaction sizes
- additional SPI slaves
- more advanced CRC/error handling
- throughput benchmarking
- deeper memory-pressure testing
- additional automated tests
- multi-device SPI architectures

These are intentionally secondary to keeping the core pipeline understandable and reliable.
