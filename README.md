# Realtime Offset Kernel Module

## Description

This Linux kernel module provides the offset between `clock_monotonic` and `clock_realtime` via a device interface. It is a simple and efficient way to retrieve the time offset between these two clocks directly from the kernel space, ensuring high precision and low latency.

## Installation

### Prerequisites

- Linux kernel headers installed. You can usually install them via your distribution's package manager. For example, on Ubuntu:
  ```sh
  sudo apt install linux-headers-$(uname -r)
  ```

### Building the Module

1. Clone the repository:
   ```sh
   git clone https://github.dev/HedgeHawk/realtime-offset.git
   cd realtime-offset
   ```

2. Build the module:
   ```sh
   make
   ```

3. Insert the module into the kernel:
   ```sh
   sudo insmod realtime_offset.ko
   ```

4. Check that the module has been loaded:
   ```sh
   lsmod | grep realtime_offset
   ```

## Usage

The module creates a device file at `/dev/realtime_offset`. You can read from this file to get the current offset between `clock_monotonic` and `clock_realtime`.

Example:
```sh
cat /dev/realtime_offset
```

## Code Overview

### Key Functions

- `offset_init`: Initializes the kernel module, registers the device class and creates the device.
- `offset_exit`: Cleans up when the module is removed, destroying the device and unregistering the device class.
- `dev_open`: Called when the device is opened.
- `dev_release`: Called when the device is released.
- `dev_read`: Reads the offset between `clock_monotonic` and `clock_realtime` and returns it to the user space.

### Device File Operations

The `file_operations` structure is defined with the following callbacks:
- `open`: Points to `dev_open`.
- `read`: Points to `dev_read`.
- `release`: Points to `dev_release`.

## License

This project is licensed under the GPL License - see the [LICENSE](LICENSE) file for details.

## Acknowledgements

Thanks to the Linux kernel community and all contributors who have helped in building and improving the kernel, making projects like this possible.