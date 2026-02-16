# FirewallManager

FirewallManager is a robust Windows Firewall management tool with a modern Qt GUI and network monitoring capabilities.

## Features
- **Rule Management**: List, add, and remove firewall rules.
- **Network Monitoring**: Track data usage per application.
- **Profiles**: Support for different firewall profiles.
- **Logging**: Detailed logs of firewall events and application activity.
- **Modern GUI**: Built with Qt 6 for a responsive user experience.
- **CLI Mode**: Full functionality available via command line for automation.

## Requirements
- Windows 10/11
- Visual Studio 2022 (with C++ development workload)
- Qt 6.5+
- CMake 3.20+

## Building
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Usage
Run as Administrator to allow firewall changes.

### CLI
```bash
FirewallManager.exe --help
FirewallManager.exe --block-all
FirewallManager.exe --allow "C:\path\to\app.exe"
```

### GUI
Simply run `FirewallManager.exe` without arguments to launch the graphical interface.

## License
MIT License
