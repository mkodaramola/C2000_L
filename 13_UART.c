#include "driverlib.h"
#include "device.h"
#include <string.h>

uint32_t ledState = 0;

void main(void)
{
    uint16_t receivedChar;
    unsigned char *msg;
    uint16_t rxStatus = 0U;

    // System & peripheral init
    Device_init();
    Device_initGPIO();

    //
    // GPIO34 as output for LED
    //
    GPIO_setPadConfig(34, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(34, GPIO_DIR_MODE_OUT);
    GPIO_writePin(34, 0);  // Start with LED off

    //
    // SCIA RX on GPIO9
    //
    GPIO_setPinConfig(GPIO_9_SCIA_RX);
    GPIO_setDirectionMode(9, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(9, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(9, GPIO_QUAL_ASYNC);

    //
    // SCIA TX on GPIO8
    //
    GPIO_setPinConfig(GPIO_8_SCIA_TX);
    GPIO_setDirectionMode(8, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(8, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(8, GPIO_QUAL_ASYNC);

    // Initialize interrupts
    Interrupt_initModule();
    Interrupt_initVectorTable();

    // SCI Configuration
    SCI_performSoftwareReset(SCIA_BASE);
    SCI_setConfig(SCIA_BASE, DEVICE_LSPCLK_FREQ, 9600,
                  (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
    SCI_resetChannels(SCIA_BASE);
    SCI_resetRxFIFO(SCIA_BASE);
    SCI_resetTxFIFO(SCIA_BASE);
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
    SCI_enableFIFO(SCIA_BASE);
    SCI_enableModule(SCIA_BASE);
    SCI_performSoftwareReset(SCIA_BASE);

#ifdef AUTOBAUD
    SCI_lockAutobaud(SCIA_BASE);
#endif

    //
    // Startup message
    //
    msg = "\r\nBluetooth SCI Ready: Send 'a' to turn ON, 'b' to turn OFF LED.\r\n";
    SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, strlen(msg));

    //
    // Main loop
    //
    for (;;)
    {
        // Wait for a char
        receivedChar = SCI_readCharBlockingFIFO(SCIA_BASE);

        // Check for errors
        rxStatus = SCI_getRxStatus(SCIA_BASE);
        if ((rxStatus & SCI_RXSTATUS_ERROR) != 0)
        {
            ESTOP0; // Break on error
        }

        // Echo back received char
        msg = "\r\nYou sent: ";
        SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, strlen(msg));
        SCI_writeCharBlockingFIFO(SCIA_BASE, receivedChar);

        // Process character
        if (receivedChar == 'a')
        {
            ledState = 1;
            msg = "\r\nLED ON\r\n";
            SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, strlen(msg));
        }
        else if (receivedChar == 'b')
        {
            ledState = 0;
            msg = "\r\nLED OFF\r\n";
            SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, strlen(msg));
        }
        else
        {
            msg = "\r\n[Unknown Command]\r\n";
            SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, strlen(msg));
        }

        // Write new LED state to GPIO34
        GPIO_writePin(34, ledState);
    }
}
