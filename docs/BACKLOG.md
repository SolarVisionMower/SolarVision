# BACKLOG

> [!IMPORTANT]
> Try not to commit to this branch! Commit in one of the sub-branches (ie `dev/xyz`) and merge as necessary

## Links to Branches

- (dev/PhysicalDesign)[https://github.com/SolarVisionMower/SolarVision/tree/dev/PhysicalDesign]
- (dev/PCBDesign)[https://github.com/SolarVisionMower/SolarVision/tree/dev/PCBDesign]
- (dev/DataCollection)[https://github.com/SolarVisionMower/SolarVision/tree/dev/DataCollection]
- (dev/Programming)[https://github.com/SolarVisionMower/SolarVision/tree/dev/Programming]
- (dev/Testing)[https://github.com/SolarVisionMower/SolarVision/tree/dev/Testing]
- (dev/Shed)[https://github.com/SolarVisionMower/SolarVision/tree/dev/Shed]
- (dev/Redocking)[https://github.com/SolarVisionMower/SolarVision/tree/dev/Redocking]

## Major Tasks

Detailed list of everything to be done, in chronological order. It should be as detailed as possible; update here and in [PROGRESS.md](./PROGRESS.md) if anything's missing.

<details>
<summary>Physical Design</summary>

Design and build of the mower's frame; all development will be in the `dev/PhysicalDesign` branch.

As of Jan 17th 2026, we have yet to discuss details about the physical design. However, the general outline will be as follows:

- Begin with a base mower, and weld/rivet on L-bracket extensions on all four corners
- On top of L-bracket posts, weld more L-brackets to form a rectangular shape
- On top of the created base, mount aluminum extrusions as needed, and weld extensions for the wheels and rear motors. [Here is some inspiration](https://youtube.com/shorts/RLpBBrU3Dhs).
- Once electronics is complete, design and 3D print enclosures for the electronics.

Along with this, we will need to build a 10S 4P battery pack out of 18650 batteries. There is also the option of purchasing a pre-built battery, but that choice will come down to cost-balance, and is yet to be discussed (01/19/2026).
</details>

<details>
<summary>PCB Design</summary>

> [!IMPORTANT]
> Please place ***ALL DOCUMENTATION*** (datasheets, reference manuals, etc.) in their respective folders under the `docs` directory.
> All designs must be based off previous designs or specs from datasheets.

All development should be in the `dev/PCBDesign` branch.

There are 3 boards which we will need to design, order, and assemble:

- STM32N6 Board
    - This will be a 4 layer board, designed in KiCAD.
    - Will use an **STM32N657I0H3Q**
        - VFBGA178 Ballout
        - 0.8mm Pitch
        - 106 I/Os (not all used; perhaps only 20, others in internal pull-down state)
        - Comm Protocols we will use:
            - UART, for communication with the ESP32
            - I2C and SPI, for collecting sensor data
            - MIPI CSI-2, for camera connection
            - XSPI, for interfacing Macronix MX25LM51245GXDI00 NOR Flash (used for boot and binary storage), same as [the Nucleo-144 board](https://www.st.com/resource/en/schematic_pack/mb1940-n657x0q-c02-schematic.pdf)
            - (*IF TIME PERMITS*) FMC_SDRAM16, for interfacing IS42S16800F-7TLI SDRAM chip, to store YOLO weights. Otherwise, use system memory with YOLOv8-nano.
            - (*IF TIME PERMITS*) FMC Display Interface, for on-board monitoring. Not needed at all, since we'll have the web-UI.
        - Of the above, CSI, XSPI, and FMC_SDRAM will have dedicated connectors on the board, while the rest will have header pins to connect to the main control board.

- Main Control Board
    - This board will house the STM32N6 board, as well as an ESP32 S3, UWB Tag, and 433MHz RF module. MCU boards connected via header pins.
    - It will be a 2-layer board for simplicity
    - Will also house LDO regulators to deliver 1.8, 3.3, and 5V respectively, powering the ESP32 and STM32
    - Within the board:
        - Route UART between the boards
        - Route *all* ESP32 pins to the main board's header pins
        - Route I2C, SPI, FMC Display, and few extra I/O pins (TBD) from STM32 to the main board's header pins

- Power Board
    - ***NOTE:*** Necessity of this board is up to debate, as off-the-shelf power components are already available, and there's an added risk of errors in our designs. **TO BE DISCUSSED (01/19/2026)**
    - This board will power the **motors, lights, and other high-power electronics**
    - This board shall have *no MCU*; all control goes directly from main board to power board.
    - Connection to main board will be through either ribbon cable, or unseparated header pin cables (resembles ribbon cable)
</details>

<details>
<summary>Data Collection/Training</summary>

> [!NOTE]
> Since this is a *software* related portion of the project, try to use dockerized tools, if applicable

All development will be in the `dev/DataCollection` branch.

The YOLOv8 model running on the STM32 will be a fine-tuned segmentation model. Our goal is that it'll be able to detect grass vs non-grass, based on input from the camera. In order to do this, we must *collect training data*, and then *train* the model on a GPU. Current plan is as follows:

- Collect 300 to 400 images of a lawn, from a camera 2 feet in the air, parallel with the earth.
- Manually annotate all the images using Label Studio
- Load dataset into Google Colab, write training script, and train for ~100 epochs.
- Export model to `pt`, use PyTorch to quantize to INT8, and export to ONNX
- Finally, utilize ST Edge AI Core to [generate C code](https://stm32ai-cs.st.com/assets/embedded-docs/stneuralart_getting_started.html), then compile in STM32Cube IDE and flash.
- Test model accuracy
</details>

<details>
<summary>Programming</summary>

> [!NOTE]
> Since this is a *software* related portion of the project, try to use dockerized tools, if applicable.
> Also, try to write *modular* code, which can be improved upon at any instance.

All development will be in the `dev/Programming` branch.

Below is how we anticipate control flow within our entire project:

- STM32
    - Camera frame is captured and processed, creating segmented frame.
    - Mask of mower's current trajectory is mapped onto the segmented frame. 
    - Finally, logic (that we've yet to define) determines distance to the lawn's boundary, if applicable.
    - Results are sent to the ESP32 via UART
    - Mower state received from ESP32 is then displayed on FMC display, or sent to Raspberry Pi base station using the 433MHz RF module.
- ESP32
    - Utilize FreeRTOS for multithreading
    - ESP32 receives distance-to-boundary predictions, and replies with the current state of the mower:
        - Position, Speed, trajectory (steering angle), Blade motor RPM, and power consumption (if possible)
        - Sent using CSV format over UART
        - Parity checker to ensure data's integrity
    - Collects sensor data from LiDAR and UWB/GPS (preferably UWB Trilateration, from other open source projects), and integrates into SLAM system.
- SLAM
    - This portion is still slightly TBD
    - Keep track of mower's position, and update in an infinite loop.
    - At beginning of mowing cycle, use path-planning algorithm to set up waypoints at boundaries of the lawn. 
    - Travel to waypoints, simultaneously updating SLAM location and map. Once boundary is detected, turn around and drive back.
- Web-UI
    - Raspberry Pi base station will run a web server and API endpoint using Python, or some other tool, TBD.
</details>

<details>
<summary>Testing</summary>

All development will be in the `dev/Testing` branch (if necessary).

This will include testing of the following (if not already done, should be already done actually):

- Motors:
    - Make sure motors run, and the ESP32 can control them as designed.
- Vision System
    - Make sure the STM32 is able to receive data from the CSI camera, is able to process it, and generate accurate results. This is crucial
- LiDAR System
    - Make sure the ESP32 is able to receive data from the LiDAR sensor, and is able to process it into the SLAM map
- UWB Positioning System
    - Make sure it can calculate position based on distance from 2 anchors. 
    - Make sure it can process current position into SLAM model.
- SLAM
    - Make sure SLAM is effectively able to update its map and position, and send commands in order to reach waypoints, in a perpetual loop.
</details>

<details>
<summary>Shed Building</summary>

All development will be in the `dev/Shed` branch.

At this point, we will know the dimensions of the mower, and will be able to construct the shed. General steps will be:

- Build/buy shed
- Install solar panels
- Install MPPT charge controller with lead acid batteries
- Permanently install Raspberry Pi Base Station, and UWB anchors.
</details>

<details>
<summary>Re-docking Programming</summary>

All development will be in the `dev/Redocking` branch.

(*IF TIME PERMITS*)

Add functionality to the code to be able to detect and return to the docking station.
</details>

## Timeline

There are 4 of us team members, and so we will be splitting up responsibilities, setting deadlines, and holding each other accountable for said deadlines. Which team member performs which tasks has not been decided yet (01/19/2026), and thus they are referred to as Member #1, #2, #3, and #4 until we have reached a consensus. Roles have been distributed as evenly as possible.

[Budgeting spreadsheet can be found here.](https://drive.google.com/drive/folders/1ZBdfXEnTgUfjmxBh2aHWwEIxdO-Y8bWZ?usp=sharing)

<!--
Table Template

| Task | Member \# | **Deadline** |
|:----:|:---------:|:------------:|
|  |  |  |
-->

- **Physical Design**

| Task | Member \# | **Deadline** |
|:----:|:---------:|:------------:|
| Welding the Frame | 1 | Feb 1 |
| Attaching physical components | 1 | Feb 8 |
| Testing DC Motors, and mower function | 1 | Feb 8 |
| Designing and 3D printing enclosures | 2 | TBD |
| Designing and Building 10S4P 18650 Battery | 1 | Feb 15 |

- **PCB Design**

| Task | Member \# | **Deadline** |
|:----:|:---------:|:------------:|
| Main Board Design, Validate, and Order | 2 | Feb 1 |
| Power Board Design, Validate, and Order (TBD) | 2 | Feb 8 |
| STM32N6 Board Design, Validate, and Order | 3 | Feb 1 |

- **Data Collection/Training**

| Task | Member \# | **Deadline** |
|:----:|:---------:|:------------:|
| Collecting Pictures | 3 | Feb 4 |
| Preliminary Training | 3 | Feb 11 |
| Testing/Iterations | 3 | Feb 21 |

- **Programming**

| Task | Member \# | **Deadline** |
|:----:|:---------:|:------------:|
| Preliminary ESP32 Programming (FreeRTOS) | 4 | Feb 1 |
| Interfacing ESP32 with LiDAR + UWB | 4 | Feb 8 |
| Model Conversion and C code generation | 4 | Feb 18 |
| C code assimilation + YOLO testing on N6 | 3 | Feb 21 |
| Preliminary SLAM Code Testing | 2 | Feb 28 |
| Path planning code testing | 2 | March 7 |
| Mower State Monitor on RbPi | 1 | Feb 28 |

- **Testing**

| Task | Member \# | **Deadline** |
|:----:|:---------:|:------------:|
| Testing Physical Components | 1 | March 7 |
| Testing ESP32 | 2 | March 14 |
| Testing STM32 | 3 | March 7 |
| Testing Communication, Web-UI, and overall mower function | 1, 2, 3, 4 | March 25 |

- **Shed Building**

| Task | Member \# | **Deadline** |
|:----:|:---------:|:------------:|
| Constructing Shed | 4 | April 1 |
| Installing Solar Panels | 4 | April 7 |
| **Senior Design Showcase Qualifications** | -- | April 7 |
| Installing MPPT Charge Controller, stationary battery pack, and mower charger | 4 | April 14 |
