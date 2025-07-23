#include "driverlib.h"
#include "device.h"


volatile uint32_t xint1Count = 0;



__interrupt void xint1ISR(void)
{
        GPIO_disableInterrupt(GPIO_INT_XINT1);
        //DEVICE_DELAY_US(1000000);


        xint1Count++; // Valid interrupt



        GPIO_enableInterrupt(GPIO_INT_XINT1);


    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}


void main(void){

    Device_init();
    Device_initGPIO();

    Interrupt_initModule();
    Interrupt_initVectorTable();

    // Configure GPIO13 as input with pull-up resistor and qualification
     GPIO_setPadConfig(13, GPIO_PIN_TYPE_STD | GPIO_PIN_TYPE_PULLUP);
     GPIO_setDirectionMode(13, GPIO_DIR_MODE_IN);
     GPIO_setQualificationMode(13, GPIO_QUAL_6SAMPLE); // 6 samples ~ debounce
     GPIO_setPinConfig(GPIO_13_GPIO13); // Set pin muxing to GPIO function

     // Map GPIO13 to XINT1
     GPIO_setInterruptPin(13, GPIO_INT_XINT1);
     GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_FALLING_EDGE);
     GPIO_enableInterrupt(GPIO_INT_XINT1);



     // Register and enable XINT1 ISR
     Interrupt_register(INT_XINT1, &xint1ISR);
     Interrupt_enable(INT_XINT1);


    EINT;
    ERTM;




   while(1){

       __asm(" NOP");


   }
}






