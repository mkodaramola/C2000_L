#include "driverlib.h"
#include "device.h"
#include "mkod.h"

uint32_t ledState = 0;

void main(void)
{
    Device_init();
    Device_initGPIO();

    GPIO_setPadConfig(34, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(34, GPIO_DIR_MODE_OUT);
    GPIO_writePin(34, 0);



    Interrupt_initModule();
    Interrupt_initVectorTable();

    sciA_init(8U, 9U, 9600);

    sciA_write("\r\nBluetooth SCI Ready: Send 'a' to turn ON, 'b' to turn OFF LED.\r\n");


    for (;;)
    {
        char c = sciA_read();

        if (c == 'a')
        {
            ledState = 1;
            sciA_write("\r\nLED ON\r\n");
        }
        else if (c == 'b')
        {
            ledState = 0;
            sciA_write("\r\nLED OFF\r\n");
        }
        else
        {
            sciA_write("\r\nUNKNOWN COMMAND\r\n");
        }


        GPIO_writePin(34, ledState);


    }// loop


}
