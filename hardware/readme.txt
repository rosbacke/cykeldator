The bike computer uses a blue-bill type of MCU breakout board.
It contains an STM32f103 micro controller with 128kB flash and 20kB SRAM.

First prototype consist of the breakout board soldered to a cheap
all-hole experiment board. Additional components:
- The 1.3" oled display.
- 2 buttons.
- a 4-pin header with: +5V, GND, pulse signal, serial TX out.

This board is mounted into a 3D printed box, strapped on to the
bike handle bar. The box can be opened for semi easy removal of the
MCU assembly. The box is designed on OpenSCAD. See 'cykeldator.scad'.

The 4 pin connector connects to a cable routed back on the bike
to a position just behind the saddle. Here there is an interconnect
between:
- Main unit cable.
- A 5V powerbank intended for mobile phones.
- 3 pin connection to the IR sensor.
- 2 pin male header for connecting a 5V serial port adapter
  to a computer. Intended for logging purposes.

The breakout board assembly can be detached and a similar connector can
be used with the development PC.

4 pin connector pinout on the board. (pin 1 closest to PCB edge):
1: (black)   GND
2: (blue)    Usart TX. (-> MCU A9)
3: (yellow)  Pulse (-> MCU A2)
4: (red)     VCC (5V)

Buttons: Close to GND. Rely on internal pullup from GPIO.
Button 1 (left) -> MCU A1.
Button 2 (right) -> MCU A0.


Development setup 1 (directly on prototype board):
------------------
- OpenOCD connected to breakout board: 4pin + reset. Requires adapter.
- USB <-> serial port adapter. Directly connected to PA9, PA10. Take power
  from USB adapter.

With this, we can develop directly on the prototype board.

Development setup 2 (naked blue-pill dev. board):
------------------
- OpenOCD connected to breakout board: 4pin + reset. Requires adapter.
- USB <-> serial port adapter. Directly connected to PA9, PA10.
- Buttons + pulse setup on external experimental board.
