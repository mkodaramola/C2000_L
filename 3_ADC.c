
#include "driverlib.h"
#include "device.h"
#include "board.h"

uint16_t adcVal =0;
void main(void)
{
    Device_init();
    Device_initGPIO();
    Board_init();

    // Initialize ADC0
    ADC_setVREF(ADCA_BASE, ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_2_0);
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCA_BASE);
    DEVICE_DELAY_US(5000);  // Wait for ADC to power up

    // Set up one SOC
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY,
                 ADC_CH_ADCIN2, 8);
    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);

    while (1)
    {
        ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);  // Start conversion

        // Wait for interrupt flag
        while (!ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1)) {}

        ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

         adcVal = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);

        // Do something with adcVal here...
        DEVICE_DELAY_US(1000); // Slow down loop
    }
}
