#include "driverlib.h"
#include "device.h"

#define INPUT_PIN              16  // GPIO16

// Globals
volatile uint32_t capEvent1 = 0;
volatile uint32_t capEvent2 = 0;
volatile uint32_t capEvent3 = 0;
volatile uint32_t capEvent4 = 0;

__interrupt void ecap1ISR(void);

void main(void)
{
    // Init device + GPIO
    Device_init();
    Device_initGPIO();

    // Setup GPIO16 for ECAP input
    GPIO_setPinConfig(GPIO_16_GPIO16);
    GPIO_setDirectionMode(INPUT_PIN, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(INPUT_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(INPUT_PIN, GPIO_QUAL_ASYNC);

    // Route GPIO16 to INPUTXBAR7
    XBAR_setInputPin(INPUTXBAR_BASE, XBAR_INPUT7, INPUT_PIN);

    // Setup ECAP1 to use INPUTXBAR7 as source
    ECAP_selectECAPInput(ECAP1_BASE, ECAP_INPUT_INPUTXBAR7);

    // ECAP1 Configuration
    ECAP_disableInterrupt(ECAP1_BASE,
        ECAP_ISR_SOURCE_CAPTURE_EVENT_1 |
        ECAP_ISR_SOURCE_CAPTURE_EVENT_2 |
        ECAP_ISR_SOURCE_CAPTURE_EVENT_3 |
        ECAP_ISR_SOURCE_CAPTURE_EVENT_4 |
        ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
        ECAP_ISR_SOURCE_COUNTER_PERIOD |
        ECAP_ISR_SOURCE_COUNTER_COMPARE);

    ECAP_clearInterrupt(ECAP1_BASE,
        ECAP_ISR_SOURCE_CAPTURE_EVENT_1 |
        ECAP_ISR_SOURCE_CAPTURE_EVENT_2 |
        ECAP_ISR_SOURCE_CAPTURE_EVENT_3 |
        ECAP_ISR_SOURCE_CAPTURE_EVENT_4 |
        ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
        ECAP_ISR_SOURCE_COUNTER_PERIOD |
        ECAP_ISR_SOURCE_COUNTER_COMPARE);


    ECAP_stopCounter(ECAP1_BASE);
    ECAP_enableCaptureMode(ECAP1_BASE);
    ECAP_setCaptureMode(ECAP1_BASE, ECAP_ONE_SHOT_CAPTURE_MODE, ECAP_EVENT_4);
    ECAP_setEventPrescaler(ECAP1_BASE, 0);

    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_1, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_2, ECAP_EVNT_RISING_EDGE);
    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_3, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_4, ECAP_EVNT_RISING_EDGE);

    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_1);
    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_2);
    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_3);
    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_4);

    ECAP_enableTimeStampCapture(ECAP1_BASE);
    ECAP_reArm(ECAP1_BASE);
    ECAP_startCounter(ECAP1_BASE);

    // Enable interrupt on 4th event
    ECAP_enableInterrupt(ECAP1_BASE, ECAP_ISR_SOURCE_CAPTURE_EVENT_4);

    // Setup interrupt
    Interrupt_initModule();
    Interrupt_initVectorTable();
    Interrupt_register(INT_ECAP1, &ecap1ISR);
    Interrupt_enable(INT_ECAP1);
    EINT;
    ERTM;

    while (1) {
        DEVICE_DELAY_US(100000);
    }
}

// ECAP1 ISR
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
