# ESP32 FreeRTOS Example – Counter, Logger, LED

## Table of Contents

**Overview**
- [Project Overview](#project-overview)
- [Project Structure](#project-structure)

**Getting Started**
- [ESP-IDF Setup](#esp-idf-setup-from-a-clean-linux-system)
- [Clone This Project](#clone-this-project)
- [Configure, Build, and Flash](#configure-build-and-flash)
- [View Logs](#view-logs-over-serial)
- [Development Workflow](#development-workflow)

**Architecture & Design**
- [Project Architecture](#project-architecture)
- [Design Notes](#design-notes)

**Additional Resources**
- [References](#references)

---

## Project Overview

This firmware implements three FreeRTOS tasks:

### Counter Task
- Increments an internal counter every 5 seconds
- Sends the incremented value to a FreeRTOS queue
- Uses a bounded queue and a short timeout to avoid blocking forever

### Logger Task
- Receives values from the queue
- Logs the received value using the standard ESP-IDF logging system
- Measures and logs the time difference between consecutive messages

### LED Blink Task
- Blinks the on-board LED once per second
- Acts as a heartbeat indicating the firmware is running

## Project Structure

```
esp32_test/
├── main/
│   ├── app_main.c
│   ├── counter_task.c
│   ├── counter_task.h
│   ├── logger_task.c
│   ├── logger_task.h
│   ├── led_task.c
│   ├── led_task.h
│   └── CMakeLists.txt
├── CMakeLists.txt
└── README.md
```

---

## ESP-IDF Setup (from a clean Linux system)

The following steps describe the complete setup starting from a fresh Linux installation (e.g., Ubuntu).

### 1. Clone ESP-IDF

```bash
cd ~
git clone --recursive https://github.com/espressif/esp-idf.git
```

**IMPORTANT:** The `--recursive` flag is mandatory. Without it, ESP-IDF submodules will be missing and build errors will appear later.

### 2. Install ESP-IDF toolchain and dependencies

```bash
cd ~/esp-idf
./install.sh esp32
```

During this step:
- The Xtensa toolchain for ESP32 is downloaded
- A Python virtual environment is created
- All required ESP-IDF Python dependencies are installed

### 3. Export ESP-IDF environment

This must be done in **every new terminal session**.

```bash
. $HOME/esp-idf/export.sh
```

After this step:
- The `idf.py` command becomes available
- The ESP-IDF Python virtual environment (idf5.x_py3.x_env) is activated

### 4. USB device detection and permissions

Verify that the ESP32 board is detected:

```bash
ls /dev/ttyUSB*
```

Add the current user to the `dialout` group:

```bash
sudo usermod -a -G dialout $USER
```

**Note:** A logout or reboot is required for the group change to take effect.

## Clone This Project

```bash
cd ~
git clone https://github.com/E2119A/ESP32-Demo esp32_test
cd esp32_test
```

## Configure, Build, and Flash

Make sure the ESP-IDF environment is exported:

```bash
. ~/esp-idf/export.sh
```

Then run:

```bash
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash
idf.py -p /dev/ttyUSB0 monitor
```

Exit the serial monitor with `Ctrl+]`.

## View Logs Over Serial

While the monitor is running, you should observe:
- LED blink messages every second
- Counter values every 5 seconds
- Logger output showing the measured time difference between messages

## Development Workflow

Typical development cycle:

1. Modify source files in `main/`
2. Build the project:
   ```bash
   idf.py build
   ```
3. Flash and monitor:
   ```bash
   idf.py flash monitor
   ```

### Important Notes

- `idf.py` must always be executed from the project root directory: `~/esp32_test`
- Do **NOT** run `idf.py` from `main/` or the home directory (`~`)

### Common Issues

**`idf.py: command not found`**  
The ESP-IDF environment was not exported:
```bash
. ~/esp-idf/export.sh
```

**`/dev/ttyUSB0: permission denied`**  
The user is not in the `dialout` group or the system was not rebooted/logged out.

---

## Project Architecture

This project demonstrates a modular producer-consumer architecture on FreeRTOS, emphasizing clean separation of concerns and deterministic real-time behavior.

### Key Design Decisions

**Task Modularity**  
Each functional unit (counter, logger, LED) is an independent FreeRTOS task with its own module. Tasks communicate through queues rather than shared state.

**Producer-Consumer Pattern**  
Counter task produces data every 5 seconds; logger task consumes from the queue. This decouples production from consumption, allowing independent operation.

**Resource Management**  
The queue is created in `app_main()` and passed explicitly to tasks. GPIO is initialized centrally before task creation, establishing clear ownership and initialization order.

**Non-Blocking Operation**  
Tasks use `vTaskDelay()` or blocking queue operations with timeouts—no busy-waiting. This prevents watchdog issues and allows efficient CPU scheduling.

**Timing & Safety**  
Logger uses `esp_timer_get_time()` for microsecond-precision measurements. Queue has fixed size (10 items) with send timeouts to handle backpressure gracefully.

### Hardware Testing

This firmware has been tested and verified on real ESP32 hardware (ESP32-DevKit V1), flashed from Ubuntu 22.04 LTS running inside Oracle VirtualBox on a Windows host, resulting in reliable 5-second counter increments, accurate timing measurements, a stable 1 Hz LED blink, and no message loss or watchdog resets.

[Demo Video](https://youtu.be/aCbNOtP_i6c)

## Design Notes

### Queue Ownership
The FreeRTOS queue is created in `app_main`. Tasks receive the queue handle explicitly, ensuring clear ownership and correct lifetime.

### Task Encapsulation
Each task is implemented in its own module with a small public API. Stack sizes and priorities are explicit and easy to adjust.

### No Busy Waiting
Tasks use `vTaskDelay()` or blocking queue operations, avoiding unnecessary CPU usage and watchdog issues.

### Timing Measurement
`esp_timer_get_time()` is used for accurate timing independent of the RTOS tick.

### Hardware Initialization
GPIO configuration is centralized in `app_main`, keeping hardware setup separate from task logic.

---

## References

- [ESP-IDF FreeRTOS API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html)
- [ESP-IDF Logging API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/log.html)
- [ESP-IDF GPIO Driver](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html)
- [ESP-IDF High Resolution Timer](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_timer.html)