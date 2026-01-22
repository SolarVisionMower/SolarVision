# Design Notes

## Power

- VDD has 7 capacitors because:
    - 3 VDD pins
    - 2 VDDIO3 pins
    - 2 VDDIO4 pins
- VDDCORE has 6 capacitors because:
    - 5 VDDCORE pins
    - 1 VDDCSI pin
- VDDA1V8 has 4 capacitors because:
    - ADC, CSI, USB, and PLL power
- ***NOTE:*** In PCB design, keep decoupling capacitors close to the BGA chip.
- Notes on Power Pinout:
    - Will obviously need a ground pin or two
    - Input voltage pin will be 5V, with a max of 3A consumption (Buck converter on main board). Use IPC-2221 to determine trace widths. 
        - The hardware development AN suggests using a 4 (if not 6) layer PCB with the inner layers being strictly ground and power planes.
    - V\_DD and V\_DDA18AON ***need to start first*** according to the boot order. The rest of the pins can be powered after. Thus:
        - V\_DD and V\_DDA18AON must be powered off two *separate* (crucial for analog signal integrity) LDO's.
        - V\_DDA1V8 will then be powered by an LDO off of the 5V input, *with PWR\_ON on the enable pin*.
        - V\_DD1V8 and V\_DD3V3 should be powered by buck converters off 5V for high power. Again *PWR\_ON TO ENABLE PIN*.
    - V\_DDA1V8PMU can be powered off V\_DD1V8 with an inductor filter in between. However, it was suggested to use a ferrite online, and so I'll be looking into that.
