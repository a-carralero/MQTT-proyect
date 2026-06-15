# MQTT Camera & Metrics System

A multi-process C++20 embedded-oriented system that streams live camera frames and system telemetry over MQTT. Built to demonstrate real-world embedded software engineering patterns: inter-process communication, real-time data pipelines, thread synchronization, and binary protocol design.

---

## Architecture

The system is composed of three independent processes that communicate exclusively through a Mosquitto MQTT broker:

```
┌─────────────────────────────────────────────────────────┐
│                      Publisher                          │
│                                                         │
│  /dev/video0 ──► capture ──► JPEG encode ──► camera/frame  │
│  /proc /sys  ──► read    ──► pack struct ──► pc/metrics │
└─────────────────────────────────────────────────────────┘
                          │
                   Mosquitto broker
                    localhost:1883
                          │
          ┌───────────────┴───────────────┐
          │                               │
┌─────────────────┐             ┌─────────────────────┐
│subscriber_camera│             │  subscriber_metrics  │
│                 │             │                      │
│ JPEG decode     │             │ memcpy → Metrics{}   │
│ rotate 180°     │             │ pretty-print to cout │
│ OpenCV display  │             └─────────────────────┘
└─────────────────┘
```

### Topics

| Topic | Payload | Rate | Description |
|---|---|---|---|
| `camera/frame` | `FrameHeader` + JPEG bytes | 30 fps | Live camera stream |
| `pc/metrics` | `Metrics` struct (packed binary) | 1 Hz | System telemetry |


---

## Project Structure

```
.
├── libs/
│   ├── Mosquitto/src/
│   │   ├── mqtt_client.hpp / .cpp   # MQTT base class (RAII, connect/disconnect)
│   │   ├── publisher.hpp            # Template publish + publish_raw
│   │   └── subscriber.hpp / .cpp    # Callback registration, loop_forever/daemon
│     
├── publisher/src/
│   ├── main_publisher.cpp           # Camera capture + metrics loop
│   └── system_monitor.hpp           # /proc and /sys readers
│
├── subscriber_camera/src/
│   ├── main_subscriber_camera.cpp   # MQTT callback + signal handling
│   ├── camera_thread.hpp            # CameraThread : ThreadMan
│   ├── thread_man.hpp               # Reusable thread lifecycle base
│   ├── thread_safe_object.hpp       # ThreadSafeObject<T>
│   └── shared_frame.hpp             # SharedFrame data type
│
└── subscriber_metrics/src/
    └── main_subscriber_metrics.cpp  # Deserialize + display Metrics
```

---

## Dependencies

| Library | Purpose | Install |
|---|---|---|
| [libmosquitto](https://mosquitto.org/) | MQTT client | `sudo apt install libmosquitto-dev mosquitto` |
| [OpenCV 4](https://opencv.org/) | Camera capture, JPEG encode/decode, image rotation | `sudo apt install libopencv-dev` |

---

## Build

Each process is compiled independently. Adjust include paths to match your project layout.
run Make from de main directory
```
---

## Run

Open four terminals:

```bash
# Terminal 1 — broker
mosquitto -v

# Terminal 2 — metrics subscriber
./subscriber_metrics

# Terminal 3 — camera subscriber
./subscriber_camera

# Terminal 4 — publisher (starts streaming)
./publisher
```

Stop with `Ctrl+C` in any terminal. The camera subscriber performs a clean shutdown via `sigwait()`.

---

## Key Concepts Demonstrated

- **Inter-process communication** via MQTT publish/subscribe over TCP
- **Binary protocol design** — packed structs, explicit framing, integrity checks
- **Real-time pipeline** — 30 fps capture → JPEG encode → MQTT publish → decode → rotate → display
- **Thread synchronization** — mutex, condition_variable, atomic, POSIX signal masking
- **Generic concurrent data structures** — `ThreadSafeObject<T>`, `ThreadMan` base class
- **Embedded C++ patterns** — stack buffers, no-heap hot paths, `const char*` interfaces, `reinterpret_cast` for binary deserialization
- **System programming** — reading `/proc/stat`, `/proc/meminfo`, `/sys/class/thermal`, V4L2 camera via OpenCV
- **MQTT protocol** — QoS levels, topic design, broker architecture, `loop_forever` vs `loop_start`
