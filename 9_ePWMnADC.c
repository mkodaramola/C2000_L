#include "driverlib.h"
#include "device.h"
#include "board.h"

#define EPWM_BASE EPWM2_BASE
#define EPWM__COUNTER_MODE EPWM_COUNTER_MODE_UP
#define EPWM_COUNTER_COMPARE EPWM_COUNTER_COMPARE_A
#define EPWM_AQ_OUTPUT EPWM_AQ_OUTPUT_A
#define EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMP EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA
#define GPIO_EPWM GPIO_2_EPWM2_A
#define GPIO_PIN 2U

#define EPWM_TIMER_TBPRD 2000
#define EPWM_MAX_CMPA    1990
#define EPWM_MIN_CMPA      0


uint16_t adcVal =0;

uint32_t cv = 0;

void initEPWM(void);
void ADCinit(void);

void main(void)
{
    Device_init();
    Device_initGPIO();


    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC); // Disable TBCLK before config

    // Setup GPIO for ePWM2A output
    GPIO_setPinConfig(GPIO_EPWM);
    GPIO_setPadConfig(GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_PIN, GPIO_QUAL_SYNC);

    Board_init();

    initEPWM();

    ADCinit();

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC); // Enable TBCLK after config



    while(1)
    {
        DEVICE_DELAY_US(1000);  // Adjust this to change fade speed

        ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);  // Start conversion

        // Wait for interrupt flag
        while (!ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1)) {}

        ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

         adcVal = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);

        cv = ((float)adcVal/4096.0)*1990.0;

        EPWM_setCounterCompareValue(EPWM_BASE, EPWM_COUNTER_COMPARE, cv);

    }


}



void initEPWM(void)
{
    EPWM_setTimeBasePeriod(EPWM_BASE, EPWM_TIMER_TBPRD);
    EPWM_setCounterCompareValue(EPWM_BASE, EPWM_COUNTER_COMPARE, EPWM_MIN_CMPA);
    EPWM_setTimeBaseCounterMode(EPWM_BASE, EPWM__COUNTER_MODE);
    EPWM_setClockPrescaler(EPWM_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);

    // Action-Qualifier: Set HIGH on CMPA up-count, LOW on zero
    EPWM_setActionQualifierAction(EPWM_BASE, EPWM_AQ_OUTPUT,
        EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMP);
    EPWM_setActionQualifierAction(EPWM_BASE, EPWM_AQ_OUTPUT,
        EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
}

void ADCinit(void){

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


}
