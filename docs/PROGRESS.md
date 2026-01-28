# PROGRESS

Documentation of all development across all branches.

[Link to BACKLOG.md](./BACKLOG.md)

> [!NOTE]
> Upon merging a development branch into this branch (`dev`), update the Completed section with the latest developments,
> and *in the proper format!* This format is `<Completed Task> - <Merged By> - <Date> - <Commit Hash>`

## TODO

- **Physical Design**
	- Designing the Frame
	- Welding the Frame
	- Attaching and Testing Components
	- Building the Battery
- **PCB Design**
	- Main Board Design
	- Power Board Design (TBD)
	- STM32N6 Board Design
- **Programming**
	- Preliminary ESP32 Programming (FreeRTOS)
	- Interfacing ESP32 with LiDAR + UWB
	- Model Conversion and C Code Generation
	- Preliminary SLAM Code Testing
	- Path Planning Code Testing
	- Mower State Monitor on RbPi

## IN PROGRESS

- **PCB Design**
	- STM32N6 Board Design
		- Added all footprints, and symbols for MX25LM, IS42, and STM32
		- Added symbols for SPX5205 LDO, and MC34064 Buck. Standard footprints.
		- Completed Power and Analog Units of schematic, as well as MX25LM NOR Flash
		- Completed pinout process in STM32CubeMX. Completed pinout is in `Software/Pinout.ioc`, and [here](https://docs.google.com/spreadsheets/d/1W3wl7rZQ2mxVOFGO9Ns9zJrD_qd8Au_sG3PKxmy8APU/edit?usp=sharing).

## COMPLETED