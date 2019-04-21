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

Available development support:
- In field programming. Ability to update firmware in field. Got serial update working,
  prepare hardware for it.
- Raw data logger. Collect measurements and dump on serial port for test/development.
  (Raw logger bandwidth: 12bytes x 200Hz x 10bits -> 24000 baud. Should be ok.)


Display: Drive small OLed. Typically controlled over i2c. Add i2c I/O expander for button
support. Plan for UI on separate board/box.

Theory of calculation:
- Create a number of small classes that are tied together. Each accepts some inputs and have a 
  'calc' function. Each also output some kind of state informing about validity of information.

Will need a number of 'clock-domains'.  Within a clock domain it should suffice with a 1 stage 
queue between modules. Between domains, there will be a need for buffering/interpolation.

 