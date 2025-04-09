# Arduino IoT Sensor Hub - Modular Organization Guide

## File Structure Overview

I've created a complete set of files to properly modularize the Arduino IoT Sensor Hub:

1. **SensorHub.ino** - Main Arduino sketch with `setup()` and `loop()`
2. **Constants.h/cpp** - Constants and pin definitions
3. **Config.h/cpp** - Configuration structure and EEPROM handling
4. **Network.h/cpp** - WiFi and MQTT connection management
5. **Sensors.h/cpp** - Sensor reading functions
6. **DataProcessing.h/cpp** - Anomaly detection and statistics
7. **Buffer.h/cpp** - Offline storage buffer management
8. **Communication.h/cpp** - MQTT publishing

## Cross-File Dependencies

Understanding the dependencies between files is crucial when splitting code:

```
SensorHub.ino
  ↓ includes all .h files
  
Constants.h ← Referenced by all other files
  
Config.h/cpp 
  ↓ uses Communication.h, Buffer.h
  
Network.h/cpp
  ↓ uses Config.h, Communication.h
  
Sensors.h/cpp
  ↓ uses Config.h, DataProcessing.h
  
DataProcessing.h/cpp
  ↓ uses Sensors.h, Config.h
  
Buffer.h/cpp
  ↓ uses Config.h, Sensors.h
  
Communication.h/cpp
  ↓ uses Sensors.h, Config.h
```

## Global Variables

Global variables are declared as `extern` in header files and defined in the corresponding cpp files:

- In the `.h` file: `extern bool networkConnected;`
- In the `.cpp` file: `bool networkConnected = false;`

When a variable needs to be accessed across multiple files, it's declared as `extern` in the header of the file where it's used.

## Compilation Order

The Arduino IDE handles compilation differently than standard C++ compilers. Here's how to avoid potential issues:

1. Make sure the `.ino` file is in a folder with the same name
2. All other files should be in the same folder
3. The Arduino pre-compiler concatenates files in a specific order:
   - First the `.ino` file
   - Then other files in alphabetical order
   
To ensure proper compilation:
- Always include the necessary header files at the top of each `.cpp` file
- Use header guards in all `.h` files
- Be careful about circular dependencies

## Using the Modular Code

To use this modular code:

1. Create a folder named `SensorHub`
2. Save all files in that folder
3. Open `SensorHub.ino` in the Arduino IDE
4. The IDE will automatically detect and include the other files in the project

## Benefits of This Organization

This modular approach offers several advantages:

1. **Maintainability**: Each file has a single responsibility
2. **Readability**: Shorter files are easier to understand
3. **Collaboration**: Multiple developers can work on different modules
4. **Reusability**: Components can be reused in other projects
5. **Testability**: Modules can be tested independently
6. **Scalability**: New features can be added with minimal changes to existing code

## Troubleshooting Common Issues

When working with modular Arduino code, watch out for:

1. **Undefined references**: Make sure all `extern` variables are defined somewhere
2. **Multiple definitions**: Check that variables aren't defined in multiple files
3. **Missing includes**: Ensure all necessary headers are included
4. **Circular dependencies**: Restructure your code to avoid them
5. **Name conflicts**: Use namespaces or prefixes to avoid conflicts