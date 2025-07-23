#include "driverlib.h"
#include "device.h"

uint16_t adcVal = 0;
uint16_t adcVal2 = 0;

__interrupt void INT_ADC_A_1_ISR(void);
__interrupt void cpuTimer0ISR(void);

void main(void)
{
    Device_init();
    Device_initGPIO();

    Interrupt_initModule();
    Interrupt_initVectorTable();

    // Register ISRs
    Interrupt_register(INT_ADCA1, &INT_ADC_A_1_ISR);
    Interrupt_register(INT_TIMER0, &cpuTimer0ISR);

    // Configure Timer0
    CPUTimer_setPeriod(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ / 1000); // 1ms
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
    CPUTimer_startTimer(CPUTIMER0_BASE);

    // Enable Timer0 interrupt
    Interrupt_enable(INT_TIMER0);

    // Configure ADC
    ADC_setVREF(ADCA_BASE, ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_2_0);
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCA_BASE);
    DEVICE_DELAY_US(5000);

    ADC_disableBurstMode(ADCA_BASE);
    ADC_setSOCPriority(ADCA_BASE, ADC_PRI_ALL_ROUND_ROBIN);

    // Setup ADC SOCs - trigger from Timer0
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_CPU1_TINT0, ADC_CH_ADCIN0, 14);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_CPU1_TINT0, ADC_CH_ADCIN10, 14);

    // SOC1 triggers ADCINT1 (end of conversion)
    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER1);
    ADC_setInterruptSOCTrigger(ADCA_BASE, ADC_SOC_NUMBER1, ADC_INT_SOC_TRIGGER_NONE); // No circular trigger
    ADC_disableContinuousMode(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);

    // Enable ADC interrupt
    Interrupt_enable(INT_ADCA1);

    EINT; // Enable Global Interrupts
    ERTM;

    while (1)
    {
        // Main loop does nothing; ADC handled via ISR
        DEVICE_DELAY_US(1000);
    }
}

__interrupt void INT_ADC_A_1_ISR(void)
{
    adcVal  = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
    adcVal2 = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);

    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

// Timer0 ISR - doesn't need to do anything unless you want it to
__interrupt void cpuTimer0ISR(void)
{
    // Just clear the flag, let it trigger ADC
    CPUTimer_clearOverflowFlag(CPUTIMER0_BASE);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
