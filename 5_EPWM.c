// Uses GPIO30 epwm1A


#include "driverlib.h"
#include "device.h"
#include "board.h"

#define EPWM1_TIMER_TBPRD 2000
#define EPWM1_MAX_CMPA    1950
#define EPWM1_MIN_CMPA      50
#define EPWM_CMP_UP          1
#define EPWM_CMP_DOWN        0

// Global variables replacing the struct
uint16_t epwmTimerIntCount = 0;
uint16_t epwmCompADirection = EPWM_CMP_UP;
volatile uint16_t compAVal;

void initEPWM1(void);
__interrupt void epwm1ISR(void);
void updateCompare(void);

void main(void)
{
    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();

    Interrupt_register(INT_EPWM1, &epwm1ISR);

    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    GPIO_setPinConfig(GPIO_30_EPWM1_A);
    GPIO_setPadConfig(30U, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(30U, GPIO_QUAL_SYNC);
    SYNC_init();
    EPWM_init(); // assumes TBPRD and mode are set in here or SysConfig
    EDIS;

    initEPWM1();
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    Interrupt_enable(INT_EPWM1);
    EINT;
    ERTM;

    while(1) {
        asm(" NOP");
    }
}

void initEPWM1(void)
{
    EPWM_setTimeBasePeriod(EPWM1_BASE, EPWM1_TIMER_TBPRD);
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, EPWM1_MIN_CMPA);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
}

__interrupt void epwm1ISR(void)
{
    updateCompare();
    EPWM_clearEventTriggerInterruptFlag(EPWM1_BASE);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

void updateCompare(void)
{
    if(epwmTimerIntCount == 10)
    {
        epwmTimerIntCount = 0;
        compAVal = EPWM_getCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A);

        if(epwmCompADirection == EPWM_CMP_UP)
        {
            if(compAVal < EPWM1_MAX_CMPA)
                EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, ++compAVal);
            else {
                epwmCompADirection = EPWM_CMP_DOWN;
                EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, --compAVal);
            }
        }
        else
        {
            if(compAVal > EPWM1_MIN_CMPA)
                EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, --compAVal);
            else {
                epwmCompADirection = EPWM_CMP_UP;
                EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, ++compAVal);
            }
        }
    }
    else
    {
        epwmTimerIntCount++;
    }
}
