#include "driverlib.h"
#include "device.h"
#include "board.h"

#define EPWM_BASE EPWM5_BASE
#define EPWM__COUNTER_MODE EPWM_COUNTER_MODE_UP
#define EPWM_COUNTER_COMPARE EPWM_COUNTER_COMPARE_B
#define EPWM_AQ_OUTPUT EPWM_AQ_OUTPUT_B
#define EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMP EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB
#define INT_EPWM INT_EPWM5
#define epwmISR epwm5ISR

#define GPIO_EPWM GPIO_17_EPWM5_B
#define GPIO_PIN 17U




#define EPWM3_TIMER_TBPRD 2000
#define EPWM3_MAX_CMPB    1950
#define EPWM3_MIN_CMPB      50


#define EPWM_CMP_UP          1
#define EPWM_CMP_DOWN        0

// Global variables
uint16_t epwmTimerIntCount = 0;
uint16_t epwmCompBDirection = EPWM_CMP_UP;
volatile uint16_t compBVal;

void initEPWM(void);
__interrupt void epwmISR(void);
void updateCompare(void);

void main(void)
{
    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();

    Interrupt_register(INT_EPWM, &epwmISR);

    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    // Setup GPIO for ePWM output
    GPIO_setPinConfig(GPIO_EPWM);
    GPIO_setPadConfig(GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_PIN, GPIO_QUAL_SYNC);

    Board_init();

    initEPWM();
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    Interrupt_enable(INT_EPWM);
    EINT;
    ERTM;

    while(1) {
        asm(" NOP");
    }
}

void initEPWM(void)
{

    EPWM_setTimeBasePeriod(EPWM_BASE, EPWM3_TIMER_TBPRD);
    EPWM_setCounterCompareValue(EPWM_BASE, EPWM_COUNTER_COMPARE, EPWM3_MIN_CMPB);
    EPWM_setTimeBaseCounterMode(EPWM_BASE, EPWM__COUNTER_MODE);
    EPWM_setClockPrescaler(EPWM_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);



    // Action-Qualifier: set output high on CMPB up, low on zero
    EPWM_setActionQualifierAction(EPWM_BASE, EPWM_AQ_OUTPUT, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMP);
    EPWM_setActionQualifierAction(EPWM_BASE, EPWM_AQ_OUTPUT, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);

    EPWM_enableInterrupt(EPWM_BASE);
    EPWM_setInterruptSource(EPWM_BASE, EPWM_INT_TBCTR_ZERO); // trigger on TBCTR = 0
    EPWM_setInterruptEventCount(EPWM_BASE, 1);
}

__interrupt void epwmISR(void)
{
    updateCompare();
    EPWM_clearEventTriggerInterruptFlag(EPWM_BASE);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

void updateCompare(void)
{
    if(epwmTimerIntCount == 10)
    {
        epwmTimerIntCount = 0;
        compBVal = EPWM_getCounterCompareValue(EPWM_BASE, EPWM_COUNTER_COMPARE);

        if(epwmCompBDirection == EPWM_CMP_UP)
        {
            if(compBVal < EPWM3_MAX_CMPB)
                EPWM_setCounterCompareValue(EPWM_BASE, EPWM_COUNTER_COMPARE, ++compBVal);
            else {
                epwmCompBDirection = EPWM_CMP_DOWN;
                EPWM_setCounterCompareValue(EPWM_BASE, EPWM_COUNTER_COMPARE, --compBVal);
            }
        }
        else
        {
            if(compBVal > EPWM3_MIN_CMPB)
                EPWM_setCounterCompareValue(EPWM_BASE, EPWM_COUNTER_COMPARE, --compBVal);
            else {
                epwmCompBDirection = EPWM_CMP_UP;
                EPWM_setCounterCompareValue(EPWM_BASE, EPWM_COUNTER_COMPARE, ++compBVal);
            }
        }
    }
    else
    {
        epwmTimerIntCount++;

}
}
