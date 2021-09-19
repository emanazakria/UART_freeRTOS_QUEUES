/******************************************************************
 *  FreeRTOS & TivaWare Example
 *
 ******************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08

uint32_t dataFromUART;        /*Variable to handle the data received from PC*/
//SemaphoreHandle_t LED_Color;  /*Semaphore handle*/

static QueueHandle_t queue=NULL;

void task1(void *pvParameters)
{

    while(1)
    {
             uint32_t received;

           if(  xQueueReceive( queue , &(received) , ( TickType_t ) 5)== pdTRUE)
           {
            if(received == 'r' || received == 'R')
            {
                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,RED);

            }
            else  if(received == 'b'|| received == 'B' )
            {
                 GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,BLUE);
            }
            else  if(received == 'g' || received == 'G' )
            {
                 GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,GREEN);
            }
            else
            {
                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x00);
            }
           }
        }
 }


void task2(void *pvParameters)
{

    while(1)
    {
        dataFromUART = UARTCharGet(UART0_BASE);    /*getting the data from UART0*/
        xQueueSend( queue,(void *)(&dataFromUART),( TickType_t ) 5);

    }
 }
void PORTF_Init (void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    //Enabling PORTF
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);    //Enabling internal LEDs
}


void UART0_Init (void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);    //Enabling PORTA
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);     //Enabling UART0

    UARTClockSourceSet(UART0_BASE,UART_CLOCK_SYSTEM);   //UART0 is working using the system clock
    UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),115200,(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));
                    /*Setting the UART baudrate, 8 bit mode, one stop bit and no parity bit*/
    UARTEnable(UART0_BASE);                         //Enabling UART0
    GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1); //Setting UART0 pins PA0,PA1
}

 void main(void)
 {

    TaskHandle_t Task_Handle_1;       //A handle for task1
    TaskHandle_t Task_Handle_2;       //A handle for task2

    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5); //80 MHz Clock


      PORTF_Init();
      UART0_Init();


    xTaskCreate(task1,"Task1",500,NULL,2,&Task_Handle_1);     //Creating the first task to control leds
    xTaskCreate(task2,"Task2",500,NULL,1,&Task_Handle_2);     //Creating the  second task to receive char from UART0

    queue=xQueueCreate( 1, sizeof( uint32_t ) );            //creating queue from two tasks communication

    vTaskStartScheduler();                                  //Starting the system*/
    while(1)
    {

    }

 }
