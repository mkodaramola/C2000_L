#include "driverlib.h"
#include "device.h"

//void delay(long double);

void main(void)
{
    // Initialize device clock and peripherals
    Device_init();

    uint32_t gpioState = 0;

    // Initialize GPIO
    Device_initGPIO();

    // Unlock GPIOs if needed
    GPIO_unlockPortConfig(GPIO_PORT_A, 0xFFFFFFFF); // Unlock all pins (optional)

    // Configure GPIO34 as output (LED)
    GPIO_setPadConfig(34, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(34, GPIO_DIR_MODE_OUT);
    GPIO_writePin(34, 0); // Start with LED off

    // Configure GPIO26 as input
    GPIO_setPadConfig(26, GPIO_PIN_TYPE_PULLUP);         // Internal pull-up
    GPIO_setDirectionMode(26, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(26, GPIO_QUAL_SYNC);       // Sync input

    // Loop Forever
    for (;;)
    {
        // Read the input pin (e.g., button or sensor)
        gpioState = GPIO_readPin(26);

        if (gpioState == 1)
        {
            // If GPIO26 is LOW (button pressed), turn ON LED
            GPIO_writePin(34, 1);

            DEVICE_DELAY_US(5000000); // 50ms
        }

        GPIO_writePin(34, 0);

    }
}



// d should be in millis
//void delay(long double d){

//    d *= 1000;

//    DEVICE_DELAY_US(d);
//}
