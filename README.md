# fsm-pump-controller-v1

## Introduction
This application is intended for the STM32F407G Discovery board. It is an automatic pump controller that reads via ADC the current battery voltage and 
the amount of liters inside the water tank. If certain conditions are met, the pump starts. If theres an error during operation, the controller will
get stuck on ERROR state until the operator resets it. This is to prevent using it when theres no water coming in.

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


