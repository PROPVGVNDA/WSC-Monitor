# WSC-Monitor
**WSC-Monitor** - is a tool developed for Windows systems, designed for educational and experimental purposes. It facilitates the injection of a dynamic link library (DLL) into a user-selected process to monitor WinAPI functions. The monitored data is then communicated back to a client application using Windows Named Pipes. The client application, in turn, exposes the received metrics to Prometheus.

## Overview
The project consists of three main modules:

### Injector Static Library
This component allows the injection of the DLL into a chosen process.
### DLL
The injected DLL monitors WinAPI functions within the target process.
### Client
Receives metrics from the injected DLL and exposes them to Prometheus-compatible interfaces.
## Features
**Dynamic Injection**: Inject the monitoring DLL into a specified process at runtime.

**WinAPI Monitoring**: Capture and monitor WinAPI function calls within the target process

**Named Pipes Communication**: Facilitate communication between the injected DLL and the client application using Windows Named Pipes.

**Prometheus Integration**: Expose monitored metrics to Prometheus-compatible interfaces for further analysis and visualization.

## Compilation
To compile the WinAPI Monitor project, follow these steps:

1. Clone Repository: Clone this repository to your local machine.
2. Open WSC-Monitor.sln
3. Run `vcpkg install` in terminal
4. Choose Debug/Release mode
5. Build Solution

## Usage
To use WinAPI Monitor, follow these steps:

1. (Optional) Export MONITOR_METRICS_DEBUG=true, to see received metrics in the console
2. Run Client.exe
3. Enter desired PID of the process you wish to monitor
4. Run Prometheus with a target at `localhost:8080`

# Disclaimer
This project is solely intended for educational and experimental purposes. It should not be used in any production or real-world environments. The developers and contributors of this project hold no liability for any misuse or damage caused by the software.
