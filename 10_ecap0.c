#include "driverlib.h"
#include "device.h"
#include "mkod.h"

#define HALL1_GPIO              8   // GPIO8

// Globals
volatile uint32_t capEvent1 = 0;
volatile uint32_t capEvent2 = 0;
volatile uint32_t capEvent3 = 0;
volatile uint32_t capEvent4 = 0;

uint32_t period = 0, dutyC = 0, freq = 0;

__interrupt void ecap1ISR(void);

void setupECAP(uint32_t ecapBase, uint32_t gpioPin, uint32_t pinConfg, XBAR_InputNum xbarInput)
{
    GPIO_setPinConfig(pinConfg);
    GPIO_setDirectionMode(gpioPin, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(gpioPin, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(gpioPin, GPIO_QUAL_ASYNC);

    XBAR_setInputPin(INPUTXBAR_BASE, (XBAR_InputNum)xbarInput, gpioPin);
    ECAP_selectECAPInput(ecapBase, (ECAP_InputCaptureSignals)(ECAP_INPUT_INPUTXBAR1 + (xbarInput)));

    ECAP_disableInterrupt(ecapBase,
        ECAP_ISR_SOURCE_CAPTURE_EVENT_1 |
        ECAP_ISR_SOURCE_CAPTURE_EVENT_2 |
        ECAP_ISR_SOURCE_CAPTURE_EVENT_3 |
        ECAP_ISR_SOURCE_CAPTURE_EVENT_4 |
        ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
        ECAP_ISR_SOURCE_COUNTER_PERIOD |
        ECAP_ISR_SOURCE_COUNTER_COMPARE);

    ECAP_clearInterrupt(ecapBase,
        ECAP_ISR_SOURCE_CAPTURE_EVENT_1 |
        ECAP_ISR_SOURCE_CAPTURE_EVENT_2 |
        ECAP_ISR_SOURCE_CAPTURE_EVENT_3 |
        ECAP_ISR_SOURCE_CAPTURE_EVENT_4 |
        ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
        ECAP_ISR_SOURCE_COUNTER_PERIOD |
        ECAP_ISR_SOURCE_COUNTER_COMPARE);

    ECAP_stopCounter(ecapBase);
    ECAP_enableCaptureMode(ecapBase);
    ECAP_setCaptureMode(ecapBase, ECAP_ONE_SHOT_CAPTURE_MODE, ECAP_EVENT_4);
    ECAP_setEventPrescaler(ecapBase, 0);

    ECAP_setEventPolarity(ecapBase, ECAP_EVENT_1, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ecapBase, ECAP_EVENT_2, ECAP_EVNT_RISING_EDGE);
    ECAP_setEventPolarity(ecapBase, ECAP_EVENT_3, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ecapBase, ECAP_EVENT_4, ECAP_EVNT_RISING_EDGE);

    ECAP_enableCounterResetOnEvent(ecapBase, ECAP_EVENT_1);
    ECAP_enableCounterResetOnEvent(ecapBase, ECAP_EVENT_2);
    ECAP_enableCounterResetOnEvent(ecapBase, ECAP_EVENT_3);
    ECAP_enableCounterResetOnEvent(ecapBase, ECAP_EVENT_4);

    ECAP_enableTimeStampCapture(ecapBase);
    ECAP_reArm(ecapBase);
    ECAP_startCounter(ecapBase);

    ECAP_enableInterrupt(ecapBase, ECAP_ISR_SOURCE_CAPTURE_EVENT_4);

    if (ecapBase == ECAP1_BASE) {
        Interrupt_register(INT_ECAP1, &ecap1ISR);
        Interrupt_enable(INT_ECAP1);
    }
}

void main(void)
{
    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();

    EPWM_t epwmB;
    epwmB.init = initEPWM;
    epwmB.output = ePWM_out;

    epwmB.init(&epwmB, 2);

    setupECAP(ECAP1_BASE, HALL1_GPIO, GPIO_8_GPIO8, XBAR_INPUT7);

    EINT;
    ERTM;

    while (1) {
        DEVICE_DELAY_US(100000);

        epwmB.output(&epwmB, 254);

        period = capEvent3 + capEvent4;
        dutyC = (100 * capEvent3) / period;
        freq = DEVICE_SYSCLK_FREQ / (period * 1);  // same as DEVICE_SYSCLK_FREQ / period
    }
}

__interrupt void ecap1ISR(void)
{
    capEvent1 = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_1);
    capEvent2 = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_2);
    capEvent3 = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_3);
    capEvent4 = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_4);

    ECAP_clearInterrupt(ECAP1_BASE, ECAP_ISR_SOURCE_CAPTURE_EVENT_4);
    ECAP_clearGlobalInterrupt(ECAP1_BASE);
    ECAP_reArm(ECAP1_BASE);

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP4);
}
