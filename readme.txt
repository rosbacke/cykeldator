This project attempt to build a bicycle computer, suitable for 
collecting statistics about bike rides.

It aims at doing a much better job of measuring distance and speed 
compared to consumer grade bike computers.

First sensor is a standard IR barrier mounted to detect the passing
of the spokes on the rear wheel.

Chosen hardware platform is a standard STM32F103 microcontroller
on a blue-pill breakout board. See:
https://satoshinm.github.io/blog/171212_stm32_blue_pill_arm_development_board_first_look_bare_metal_programming.html


Primay function is to measure speed with high accuracy and log data.
Using  a photo sensor measuring time between spokes, we get about 30 reading per turn.

Need development support:
- In field programming. Ability to update firmware in field. Got serial update working,
  prepare hardware for it.
- Raw data logger. Collect measurements and dump on serial port for test/development.
So: Will need serial port bootloading fairly soon. Will need serial consol with a 
PC connection in a bag on the bike. (Missing STLink V2. Need that or serial bootloading.)
(Raw logger bandwidth: 12bytes x 200Hz x 10bits -> 24000 baud. Should be ok.)
- Set up serial port hardware with 6 pin connector as first approx. Can use boot0 jumper to
  program using stm32flash and standard serial adapter. (Done)
- Use battery bank for power.
- Use some 3 pin connector for opto connection.
- Set up: Opto sensor on bike, Start with weak glue and straps. 
  Prepare: Ordinary LED to monitor output during mounting. In the end: want a 3-pole screw connector 
  for the signal.
- Prepare board with cables o be connected to 3 pole screw connector.


Display: Drive small OLed. Typically controlled over i2c. Add i2c I/O expander for button
support. Plan for UI on separate board/box.

Theory of calculation:
- Create a number of small classes that are tied together. Each accepts some inputs and have a 
  'calc' function. Each also output some kind of state informing about validity of information.

Will need a number of 'clock-domains'.  Within a clock domain it should suffice with a 1 stage 
queue between modules. Between domains, there will be a need for buffering/interpolation.





Interrupt helpers:
- Set up each interrupt with a specific interrupt level.
- Define a class which list a number interrupts that can access it. It calculates
  a maximum priority level to use for protection.

- Create 'covers'. Name ISR that do the locking and generate code from that. Runtime will check
  that the correct ISR is running of fail. Cover and then restore mask given ISR priority.
- Wildcard covers will fail if incoming BASEPRI is to high. Othervise store current pri, protect
  and restore prev runtime value.
