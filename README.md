# STM32F4 Stopwatch (hand coded FSM)

Stopwatch application for the STM32F4-Discovery board under Erika OS (Real-Time Operating System). The stopwatch core functionalities have been modelled using a FSM which has been implemented in code using a nested switch pattern. The application has been displayed on the touchpad extension for the Discovery board.

## Getting Started

In order to compile the application, you need:

* [Erika Enterprise](http://erika.tuxfamily.org/drupal/download.html) - OS used on top of the STM32F4-Discovery board.

I suggest to download the IDE version and simply import this project in the IDE and compile it from there.

The obtained `.elf` file can be used to flash the board.

## Running the application

The application will automatically start after flashing the binary obtained at the previous step.

## Author

* **Paolo Sassi** - [paulrox](https://github.com/paulrox)
