# MultiCast_Module


## Overview

MultiCast_Module is a network application framework that enables seamless communication between multicast UDP groups and TCP clients. It uses an event-driven architecture based on the Reactor pattern with epoll for efficient I/O multiplexing.

### Key Features

- **Event-Driven Architecture** - Uses the Reactor pattern with epoll for scalable I/O handling
- **Multicast Support** - Join multicast groups and send/receive UDP multicast messages
- **TCP Server** - Handles multiple TCP client connections simultaneously
- **Timer Management** - Supports periodic and one-shot timers
- **Thread-Safe Message Processing** - Uses producer-consumer pattern for thread-safe message handling
- **Extensible Design** - Clean interface-based architecture for easy customization

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         Main Application                        │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │   Reactor    │  │  TimerMgr    │  │ MulticastMgr │          │
│  │   (epoll)    │  │              │  │              │          │
│  └──────┬───────┘  └──────────────┘  └──────────────┘          │
│         │                                                        │
│  ┌──────┴───────┐                                               │
│  │   TcpManager │                                               │
│  │              │                                               │
│  └──────┬───────┘                                               │
│         │                                                        │
│  ┌──────┴───────┐                                               │
│  │   IApplication (User Code)                                  │
└──────────────────────────────────────────────────────────────────┘
```

## Project Structure

```
MultiCast_Module/
├── src/
│   ├── Application/          # User application implementation
│   │   ├── IApplication.h    # Application interface
│   │   └── Application.cpp   # Application implementation
│   ├── Reactor/              # Reactor pattern implementation
│   │   ├── Reactor.h/cpp     # Main reactor with epoll
│   │   └── IEventHandler.h   # Event handler interface
│   ├── Timer/                # Timer management
│   │   ├── TimerManager.h/cpp
│   │   └── TimerType.h       # Timer ID definitions
│   ├── Tcp/                  # TCP server implementation
│   │   ├── TcpManager.h/cpp  # TCP server with multiple clients
│   │   └── ClientSession.h/cpp
│   ├── Multicast/            # Multicast UDP handling
│   │   ├── MulticastManager.h/cpp
│   │   └── MulticastManager.cpp
│   ├── Log/                  # Logging utilities
│   │   ├── log.h/cpp
│   │   └── spdlog/           # Third-party logging library
│   └── main.cpp              # Application entry point
├── build/                    # Build directory
├── CMakeLists.txt            # CMake configuration
└── README.md                 # This file
```

## Building

### Prerequisites

- CMake >= 3.16
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2019+)
- Linux with epoll support (or compatible)

### Build Steps

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
make
```

### Build Options

- `-DCMAKE_BUILD_TYPE=Release` - Release build (default)
- `-DCMAKE_BUILD_TYPE=Debug` - Debug build
- `-DBUILD_SHARED_LIBS=ON` - Build shared libraries
- `-DENABLE_UNIT_TESTS=ON` - Enable unit tests

## Usage

### Basic Application Setup

```cpp
#include "Reactor.h"
#include "TimerManager.h"
#include "MulticastManager.h"
#include "TcpManager.h"
#include "Application.cpp"

int main() {
    Logger::init();
    
    Reactor reactor;
    Application app;
    
    // Setup Timer
    TimerManager timerManager(reactor, &app);
    timerManager.startTimer(TimerId::Timer1, 5);
    
    // Setup TCP Server
    TcpManager tcpManager(&app, reactor);
    tcpManager.startServer(13400);
    
    // Setup Multicast
    MulticastManager mcast(reactor, &app);
    mcast.joinGroup("239.1.1.1", 13400);
    
    // Run event loop
    reactor.run();
}
```

### IApplication Interface

Implement the `IApplication` interface to handle events:

```cpp
class Application : public IApplication {
public:
    void onTimerExpired(int timerId) override;
    void onClientConnected(int clientFd) override;
    void onClientDisconnected(int clientFd) override;
    void onMessageReceived(ClientSession* session, const char* data) override;
    void onMultiCastData(const char* group, const char* data) override;
    void sendToClient(const std::string& data) override;
};
```

## Components

### Reactor

The Reactor is the core event dispatcher using epoll for I/O multiplexing. It manages file descriptors and dispatches events to registered handlers.

**Key Methods:**
- `registerFd()` - Register a file descriptor with event types
- `unregisteredFd()` - Unregister a file descriptor
- `run()` - Start the event loop

### TimerManager

Manages periodic and one-shot timers. Timers use the Reactor's epoll with timerfd for efficient timing.

**Key Methods:**
- `startTimer()` - Start a timer with specified interval
- `stopTimer()` - Stop a timer

### TcpManager

TCP server that accepts multiple client connections. Each connection is managed by a `ClientSession`.

**Key Methods:**
- `startServer()` - Start TCP server on specified port
- `removeClient()` - Disconnect a client
- `acceptClient()` - Accept new connections

### MulticastManager

Handles UDP multicast group membership and message sending.

**Key Methods:**
- `joinGroup()` - Join a multicast group
- `sendtoGroup()` - Send message to multicast group

## Event Flow

```
┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│   Multicast  │      │   TCP Client │      │    Timer     │
│    Data      │      │    Data      │      │    Expired   │
└──────┬───────┘      └──────┬───────┘      └──────┬───────┘
       │                     │                     │
       └─────────────────────┼─────────────────────┘
                             │
                      ┌──────▼───────┐
                      │   Reactor    │
                      │   (epoll)    │
                      └──────┬───────┘
                             │
                    ┌────────▼────────┐
                    │  IApplication   │
                    │   Callbacks     │
                    └─────────────────┘
```

## Network Configuration

### Multicast Group

- **Group IP**: 239.1.1.1 (239.0.0.0/8 - GLOP scope)
- **Port**: 13400
- **Protocol**: UDP

### TCP Server

- **Port**: 13400 (configurable)
- **Protocol**: TCP

## Thread Safety

The Application class uses a thread-safe message queue with mutex and condition variable for processing requests in a separate thread.

## Logging

Uses **spdlog** for logging with the following levels:
- `Logger::debug()` - Debug messages
- `Logger::info()` - Informational messages
- `Logger::warn()` - Warning messages
- `Logger::error()` - Error messages

## Performance Considerations

- **Non-blocking I/O** - All file descriptors are set to non-blocking mode
- **Edge-triggered epoll** - Uses EPOLLET for efficient event notification
- **Connection pooling** - TCP clients managed efficiently
- **Buffer management** - Dynamic buffer resizing for variable message sizes

## License

This project is provided as-is for educational and production use.

## Contributing

Contributions are welcome! Please ensure:
- Code follows existing style
- New features include appropriate tests
- Documentation is updated
