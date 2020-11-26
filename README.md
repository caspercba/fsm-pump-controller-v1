# Finite State Machine Pump Controller v1

## Introduction
This application is intended for the STM32F407G Discovery board. It is an automatic pump controller that reads via ADC the current battery voltage and 
the amount of liters inside the water tank. If certain conditions are met, the pump starts. If theres an error during operation, the controller will
get stuck on ERROR state until the operator resets it. This is to prevent using it when theres no water coming in.

## External hardware
The pump: We need a pump to turn on and off
Water level sensor: A pressure driven water level sensor is used to determine the water column, then we estimate the amount of water we have stored.
Relay: One relay to drive the pump.
4 buttons: Used mainly for the internal menu, to set all the parameters required, including tank capacity, time, date, etc.
LCD SCreen: 16x2 hitachi-like LCD backlit screen (i2c), but it could be easily adapted to other lcd models.
PSU: The water pressure sensor requires at least 12v to work
5v PSU: This is for the STM controller, we might just drop down the 12v from the main supply using any generic voltage reguator.


## Motivation
This is a small project created for a university course "Embedded systems design", but it has a real world application. 
The problem: We have a very remote house powered by solar energy. The water supply comes from a pump installed in a nearby river and gets the water up
to 90 meters where the 8000L tank is located. When the pump fails to deliver enough water due to clogged filters or pipes disconnected, it tends to run
for days. Because this pump is very power hungry (1300w approx), letting it work for days not being able to fill the tanks is a big problem. It usually
drains our 800Ah (48v) battery bank, and also it runs the risk of damaging itself due to heat.
Solution: This project. It will automatically turn on the pump during the set times (ideally during the day, so we dont use the stored power but solar),
and when the batteries have some minimum voltage. We also check with a walter level sensor that water is really coming in. Theres a shortcoming to be solved though, if someone is consuming too much water and we are filling the tanks, we might thing that the pump rate (liters/min) is too low, and trigger an 
auto shutdown. Maybe it would be wise to add a flow sensor instead of relying on this single pressure sensor.

## The details.

Create a robust app that allows easy threading and layer separation between the logic and the hardware board.

### Layers: 
Theres a bottom layer called bsp.c that implements the interface bsp.h. This interface provides all the necessary services to the top layers so they
can decide the logic. This separation allows future migration to other platforms.

### Threading: 
This is an interrupt driven application. Each interrupt does call a configurable callback that will then throw an event to the system. Each
event is stored in a circular buffer (circular queue). Everytime the main thread calls getNext(), events will be removed from the queue and processed.

### FSM: 
Theres a small state machine that acts when the main thread calls `fsm_handle_events()`. What we want to do is to keep the `int main(void)` code
small, and do most of the logic in our state machine. The state machine was designed by me. The main issues we have is that all memory is preallocated in
the stack, so it is not as efficient as it could be. We dont care at this point since this is not a very heavy app.

### lcd_helper: 
This component just helps the state machine to show information in our LCD screen. `lcd_helper` will eventually call an lcd lib that handles
all the boilerplate to show stuff.

### USART2: 
Every `printf` sentence is redirected to the USART2 port for debugging purposes. We need the trailing \r\n in order to show properly the data via the serial
port. The main reason being that every char printed on printf is forwarded to `__io_putchar` since this works on a char-to-char basis, theres no way we can
"inject" the end of line chars, because we never know where is the end. 
The lines of code that perform such forwarding action are the following:

```
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

```

## Plans for the future
I'm in the process of developing a new version that will work on FreeRTOS. Considering that I designed this application in a multi-threaded manner, it makes
sense to migrate it to FreeRTOS. But dont worry, this project will stay here for future references. It will probably be started as a fork instead.


## Notes
The code quality is probably not top notch. Im trying to understand the C programming language. You will notice theres not a single malloc/free sentence, theres a reason for this: I dont know how to properly do dynamic memory allocation. This is another improvement we could make, specially the circular buffer that is completely heap driven, not a good idea.



