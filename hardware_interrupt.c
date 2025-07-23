#include "driverlib.h"
#include "device.h"

// Interrupt counter
volatile uint32_t xint1Count = 0;

// ISR prototype
__interrupt void xint1ISR(void);

void main(void)
{
    // Initialize device and clock
    Device_init();

    // Initialize interrupt module and vector table
    Interrupt_initModule();
    Interrupt_initVectorTable();

    // Register XINT1 ISR
    Interrupt_register(INT_XINT1, &xint1ISR);

    // Set GPIO13 as input
    GPIO_setDirectionMode(13, GPIO_DIR_MODE_IN);

    // Use SYNC qualification (fastest)
    GPIO_setQualificationMode(13, GPIO_QUAL_SYNC);

    // Map GPIO13 to XINT1
    GPIO_setInterruptPin(13, GPIO_INT_XINT1);

    // Trigger interrupt on falling edge
    GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_FALLING_EDGE);

    // Enable GPIO interrupt for XINT1
    GPIO_enableInterrupt(GPIO_INT_XINT1);

    // Enable group interrupt in PIE and global interrupts
    Interrupt_enable(INT_XINT1);
    EINT;
    ERTM;

    // GPIO31 as output (for debugging LED or oscilloscope)
    GPIO_setDirectionMode(31, GPIO_DIR_MODE_OUT);
    GPIO_writePin(31, 0);

    // Main loop (idle)
    while(1)
    {
        DEVICE_DELAY_US(1000); // Do nothing, wait for interrupt
    }
}

//
// XINT1 ISR
//
__interrupt void xint1ISR(void)
{
    xint1Count++; // Increment counter for debug

    // Toggle GPIO31
    GPIO_togglePin(31);

    // Acknowledge interrupt to PIE group 1
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
