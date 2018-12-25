# BMK-SDK
Bluetooth mechanical keyboard firmware for nRF52 SoC using nRF5 SDK

## Features
* [x] Basic functionality.
    * [x] Basic keys.
    * [x] Shifted keys.
    * [x] Multi-layer support.
    * [x] **(TESTING)** Master-to-slave link.
* [ ] Devices connectivity. Can connect up to 3 (or more) devices and switch between them.
* [ ] Low power mode.
* [ ] Media keys.

## Supported board.
* BlueMicro

## Supported keyboard.
* ErgoTravel

## Setup
1. Download and install [SEGGER Embedded Studio](https://www.segger.com/products/development-tools/embedded-studio).
2. Download and extract [nRF5 SDK](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK) into a proper folder along with this project. It should look like this:
    ```
    .
    +-- nRF5_SDK (This folder contains all contents of nRF5 SDK.)
    |   +-- components
    |   +-- config
    |   +-- documentation
    |   +-- ...
    +-- bmk-sdk (This project folder.)
        +-- src
        +-- ...
    ```
    All referenced source files and headers will be resolved to folder nRF5_SDK above.
3. Open project file (.emProject) using SEGGER Embedded Studio.
4. Build and flash your firmware.

## Tutorials on how to use SEGGER Embedded Studio
* [The complete cross-platform nRF development tutorial](https://www.novelbits.io/cross-platform-nrf-development-tutorial)
* [Getting started with SEGGER Embedded Studio and the nRF5 SDK](https://www.youtube.com/playlist?list=PLx_tBuQ_KSqGHmzdEL2GWEOeix-S5rgTV)
