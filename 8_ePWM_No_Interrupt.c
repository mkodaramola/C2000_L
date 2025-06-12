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
#define EPWM_MAX_CMPA    1950
#define EPWM_MIN_CMPA      50

#define EPWM_CMP_UP    1
#define EPWM_CMP_DOWN  0

// Global variables
uint16_t epwmCompADirection = EPWM_CMP_UP;
volatile uint16_t compAVal = EPWM_MIN_CMPA;

void initEPWM(void);
void updateCompare(void);

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

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC); // Enable TBCLK after config

    while(1)
    {
        DEVICE_DELAY_US(1000);  // Adjust this to change fade speed
        updateCompare();
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

void updateCompare(void)
{
    if(epwmCompADirection == EPWM_CMP_UP)
    {
        if(compAVal < EPWM_MAX_CMPA)
        {
            compAVal++;
        }
        else
        {
            epwmCompADirection = EPWM_CMP_DOWN;
            compAVal--;
        }
    }
    else
    {
        if(compAVal > EPWM_MIN_CMPA)
        {
            compAVal--;
        }
        else
        {
            epwmCompADirection = EPWM_CMP_UP;
            compAVal++;
        }
    }

    EPWM_setCounterCompareValue(EPWM_BASE, EPWM_COUNTER_COMPARE, compAVal);
}
