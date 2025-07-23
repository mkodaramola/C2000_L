
#include "driverlib.h"
#include "device.h"

uint16_t adcVal =0;
uint16_t adcVal2 =0;
void main(void)
{
    Device_init();
    Device_initGPIO();

    // Initialize ADC0
    ADC_setVREF(ADCA_BASE, ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_2_0);
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCA_BASE);
    DEVICE_DELAY_US(5000);

    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY,
                 ADC_CH_ADCIN0, 8);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY,
                 ADC_CH_ADCIN10, 8);


    while (1)
    {
        ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);
        ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER1);


         adcVal = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
         adcVal2 = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);


        DEVICE_DELAY_US(1000);
    }
}

