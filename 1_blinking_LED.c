#include "driverlib.h"
#include "device.h"
void main(void)
{
    // Initialize device clock and peripherals
    Device_init();

    // Initialize GPIO and configure the GPIO pin as a push-pull output
    Device_initGPIO();
    GPIO_setPadConfig(34, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(34, GPIO_DIR_MODE_OUT);

    GPIO_writePin(34, 0);

// Loop Forever
for(;;){


    // Turn off LED
    GPIO_writePin(34, 0);

    // Delay for a bit.
    DEVICE_DELAY_US(500000);

    // Turn on LED
    GPIO_writePin(34, 1);

    // Delay for a bit.
    DEVICE_DELAY_US(500000);

}
}
