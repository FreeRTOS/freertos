## Overview

This directory provides an SMP FreeRTOS-Kernel port that can be used with the Raspberry Pi Pico SDK. It supports:

 * Simple CMake INTERFACE libraries, to provide the FreeRTOS-Kernel and also the individual allocator types, without copying code into the user's project.
 * Running the FreeRTOS-Kernel and tasks on either core 0 or core 1, or both.
 * Use of SDK synchronization primitives (such as mutexes, semaphores, queues from pico_sync) between FreeRTOS tasks and code executing on a non FreeRTOS core, or in IRQ handlers.

Note that whilst this SMP version can be run on just a single (either) core, it is probably
more efficient to use the non SMP version in the main FreeRTOS-Kernel branch in that case.

## Using this port

You can copy [FreeRTOS-Kernel-import.cmake](FreeRTOS-Kernel-import.cmake) into your project, and
add the following in your `CMakeLists.txt`:

```cmake
import(FreeRTOS_Kernel_import.cmake)
```

This will locate the FreeRTOS kernel if it is a direct sub-module of your project, or if you provide the 
`FREERTOS_KERNEL_PATH` variable in your environment or via `-DFREERTOS_KERNEL_PATH=/path/to/FreeRTOS-Kernel` on the CMake command line.

**NOTE:** If you are using version 1.3.1 or older of the Raspberry Pi Pico SDK then this line must appear before the 
`pico_sdk_init()` and will cause FreeRTOS to be included/required in all RP2040 targets in your project. After this SDK 
version, you can include the FreeRTOS-Kernel support later in your CMake build (possibly in a subdirectory) and the 
FreeRTOS-Kernel support will only apply to those targets which explicitly include FreeRTOS support.

As an alternative to the `import` statement above, you can just add this directory directly via thw following (with 
the same placement restrictions related to the Raspberry Pi Pico SDK version above): 

```cmake
add_subdirectory(path/to/this/directory FreeRTOS-Kernel)
```


## Advanced Configuration

Some additional `config` options are defined [here](include/rp2040_config.h) which control some low level implementation details.

## Known Limitations

- Tickless idle has not currently been tested, and is likely non-functional