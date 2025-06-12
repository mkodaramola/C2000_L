// Uses GPIO5 epwm3B

#include "driverlib.h"
#include "device.h"
#include "board.h"

#define EPWM3_TIMER_TBPRD 2000
#define EPWM3_MAX_CMPB    1950
#define EPWM3_MIN_CMPB      50
#define EPWM_CMP_UP          1
#define EPWM_CMP_DOWN        0

// Global variables
uint16_t epwmTimerIntCount = 0;
uint16_t epwmCompBDirection = EPWM_CMP_UP;
volatile uint16_t compBVal;

void initEPWM3(void);
__interrupt void epwm3ISR(void);
void updateCompare(void);

void main(void)
{
    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();

    Interrupt_register(INT_EPWM3, &epwm3ISR);

    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    // 💡 Setup GPIO5 for ePWM3B output
    GPIO_setPinConfig(GPIO_5_EPWM3_B);
    GPIO_setPadConfig(5U, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(5U, GPIO_QUAL_SYNC);

    SYNC_init();
    EPWM_init();
    EDIS;

    initEPWM3();
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    Interrupt_enable(INT_EPWM3);
    EINT;
    ERTM;

    while(1) {
        asm(" NOP");
    }
}

void initEPWM3(void)
{
    EPWM_setTimeBasePeriod(EPWM3_BASE, EPWM3_TIMER_TBPRD);
    EPWM_setCounterCompareValue(EPWM3_BASE, EPWM_COUNTER_COMPARE_B, EPWM3_MIN_CMPB);
    EPWM_setTimeBaseCounterMode(EPWM3_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_setClockPrescaler(EPWM3_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);

    // 🔧 Action-Qualifier: set output high on CMPB up, low on zero
    EPWM_setActionQualifierAction(EPWM3_BASE, EPWM_AQ_OUTPUT_B,
        EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(EPWM3_BASE, EPWM_AQ_OUTPUT_B,
        EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
}

__interrupt void epwm3ISR(void)
{
    updateCompare();
    EPWM_clearEventTriggerInterruptFlag(EPWM3_BASE);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

void updateCompare(void)
{
    if(epwmTimerIntCount == 10)
    {
        epwmTimerIntCount = 0;
        compBVal = EPWM_getCounterCompareValue(EPWM3_BASE, EPWM_COUNTER_COMPARE_B);

        if(epwmCompBDirection == EPWM_CMP_UP)
        {
            if(compBVal < EPWM3_MAX_CMPB)
                EPWM_setCounterCompareValue(EPWM3_BASE, EPWM_COUNTER_COMPARE_B, ++compBVal);
            else {
                epwmCompBDirection = EPWM_CMP_DOWN;
                EPWM_setCounterCompareValue(EPWM3_BASE, EPWM_COUNTER_COMPARE_B, --compBVal);
            }
        }
        else
        {
            if(compBVal > EPWM3_MIN_CMPB)
                EPWM_setCounterCompareValue(EPWM3_BASE, EPWM_COUNTER_COMPARE_B, --compBVal);
            else {
                epwmCompBDirection = EPWM_CMP_UP;
                EPWM_setCounterCompareValue(EPWM3_BASE, EPWM_COUNTER_COMPARE_B, ++compBVal);
            }
        }
    }
    else
    {
        epwmTimerIntCount++;
    }
}
