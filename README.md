# SolarVision Mower

> [!NOTE]
> *This project is still in development!*

## Project Overview

SolarVision Mower is a YOLO and LiDAR based autonomous lawnmower that we're building for our final capstone engineering project. Once complete, it will feature *fully custom hardware and software*, which includes custom circuitry, firmware, 3D printed or welded components, a modified frame from an existing TORO lawnmower, and much more.

To stay up-to-date with the latest developments, check out the [dev/general][01] branch (specifically the [BACKLOG.md][02] file), to find a detailed list of everything we're doing. It will also help to navigate the various branches and directories (which will most likely grow in complexity as time progresses). Branches will the naming convention `xx/yy` are *permanent* branches, in the category specified by `xx`. Others can be considered temporary only.

## Tech Stack

- Software Tools
    - **KiCAD** - For PCB Design
    - **Fusion360** - For designing 3D models
    - **ESP-IDF** - For firmware programming on the ESP32
    - **STM32CubeMX** - For hardware configuration and initial code generation on the STM32
    - **STM32CubeIDE** - For firmware programming on the STM32
    - **ST Edge AI Suite** - For YOLO model conversion and code generation
- Software Libraries
    - **FreeRTOS** - For multithreading on both microcontrollers
    - **[espressif/arduino-esp32][05]** - For running Arduino functions in the ESP32
    - **[thijses/rplidar][06]** - For ease of integration of the RPLidar A1.
    - **[jh247247/DW1000-fan-video][07]** - Initial firmware implementation for the Ultra Wideband Positioning Modules
- Hardware Tools
    - Fabrication tools from the [NJIT Makerspace][08].
    - Circuit analysis tools from the [NJIT ECE Labs][09].
    - SLogic16U3 Logic Analyzer.
    - [JLCPCB][10] for PCB fabrication

## The Team

- Arnav Revankar
    - **Computer Engineer** - Graduating Dec 2026
    - Responsibilities:
        - Co-Lead Developer, and Manager for Hardware Design.
        - Developed the [STM32N6 Board][03]
        - Oversaw the development of the [Main Board][04]
- Panuha Were
    - **Computer Engineer** - Graduating May 2026
    - Responsibilities:
        - Co-Lead Developer, and Manager for Software Design
        - Contributed to the [Main Board][04] hardware design
        - Overseeing Firmware Development for the ESP32S3
- Layth Younes 
    - **Computer Engineer** - Graduating May 2026
    - Responsibilities:
        - Developer, and Manager for 3D Designs and Fabrication
        - Contributed to the [Main Board][04] hardware design
        - Overseeing the physical design of the frame, and enclosures.
- Daniel Assaf
    - **Computer Engineer** - Graduating May 2026
    - Responsibilities:
        - Developer, and Test/Integration Engineer
        - Contributed to the [Main Board][04] hardware design
        - Contributed to writing Documentation/Reports on progress.

## License

As of date (March 2nd, 2026) we have *not* decided upon a license. All rights are reserved by the authors.

<!-- Links -->
[01]: https://github.com/SolarVision/SolarVisionMower/tree/dev/general
[02]: https://github.com/SolarVision/SolarVisionMower/blob/dev/general/docs/BACKLOG.md
[03]: https://github.com/SolarVision/SolarVisionMower/tree/dev/PCBDesign/Hardware/STM32N6_Board
[04]: https://github.com/SolarVision/SolarVisionMower/tree/dev/MainBoard/Hardware/Main_Board
[05]: https://github.com/espressif/arduino-esp32
[06]: https://github.com/thijses/rplidar
[07]: https://github.com/jh247247/DW1000-fan-video
[08]: https://njitmakerspace.com
[09]: https://ecelabs.njit.edu
[10]: https://jlcpcb.com 
