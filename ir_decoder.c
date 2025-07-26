#include "driverlib.h"
#include "device.h"

#define HALL1_GPIO              8   // GPIO8

volatile float period_ms = 0;
volatile uint32_t capEvent1 = 0;
volatile uint32_t capEvent2 = 0;
volatile uint32_t fallingEdgeDelta = 0;

volatile uint32_t c = 0;
volatile uint32_t cp = 0;
volatile uint8_t data = 0;




__interrupt void ecap1ISR(void);
__interrupt void cpuTimer0ISR(void);


void initCPUTimers(void);
void configCPUTimer(uint32_t, float, float);
void setupECAP(uint32_t, uint32_t, uint32_t, XBAR_InputNum);





void main(void)
{
    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();


    Interrupt_register(INT_TIMER0, &cpuTimer0ISR);

    initCPUTimers();
    configCPUTimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, 3000);
    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
    Interrupt_enable(INT_TIMER0);
    CPUTimer_startTimer(CPUTIMER0_BASE);

    setupECAP(ECAP1_BASE, HALL1_GPIO, GPIO_8_GPIO8, XBAR_INPUT7);

    GPIO_setPinConfig(GPIO_34_GPIO34);
    GPIO_setPadConfig(15U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(15U, GPIO_DIR_MODE_OUT);

    EINT;
    ERTM;

    while (1) {

        if (data == 16) GPIO_writePin(15U, 1);

        if (data == 1) GPIO_writePin(15U, 0);

        DEVICE_DELAY_US(2000);


    }
}

__interrupt void ecap1ISR(void)
{




    capEvent1 = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_1);
    capEvent2 = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_2);

    fallingEdgeDelta = capEvent2; // Since counter resets on event 1

    period_ms = ((float)capEvent2 * 1000.0f)/(float)DEVICE_SYSCLK_FREQ;

    if (c == 1) data = 0;

    if(c >= 9 && c <= 13) {


        if (period_ms >= 1.0f && period_ms <= 1.5f)data = (data << 1); // Logic 1 = 1.125ms
        else if(period_ms >= 2.0f && period_ms <= 2.5f) data = (data << 1) | 1; // Logic 0 = 2.25ms

    }


    c++;

    c = (c > 17) ? 0:c;



    ECAP_clearInterrupt(ECAP1_BASE, ECAP_ISR_SOURCE_CAPTURE_EVENT_2);
    ECAP_clearGlobalInterrupt(ECAP1_BASE);
    ECAP_reArm(ECAP1_BASE);

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP4);


}




void initCPUTimers(void)
{
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_stopTimer(CPUTIMER0_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
}

void configCPUTimer(uint32_t base, float cpuFreqHz, float periodMs)
{
    uint32_t periodCount = (uint32_t)((cpuFreqHz / 1000.0f) * periodMs);
    CPUTimer_setPeriod(base, periodCount - 1);
    CPUTimer_setPreScaler(base, 0);
    CPUTimer_stopTimer(base);
    CPUTimer_reloadTimerCounter(base);
    CPUTimer_setEmulationMode(base, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(base);
}

__interrupt void cpuTimer0ISR(void)
{

    if (c != 0) c = 0;

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}


void setupECAP(uint32_t ecapBase, uint32_t gpioPin, uint32_t pinConfg, XBAR_InputNum xbarInput)
{
    // Configure GPIO as ECAP input
    GPIO_setPinConfig(pinConfg);
    GPIO_setDirectionMode(gpioPin, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(gpioPin, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(gpioPin, GPIO_QUAL_ASYNC);

    // Connect GPIO to ECAP input via XBAR
    XBAR_setInputPin(INPUTXBAR_BASE, xbarInput, gpioPin);
    ECAP_selectECAPInput(ecapBase, (ECAP_InputCaptureSignals)(ECAP_INPUT_INPUTXBAR1 + xbarInput));

    // Disable & clear all interrupts
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
    // Stop + configure counter
    ECAP_stopCounter(ecapBase);
    ECAP_enableCaptureMode(ecapBase);
    ECAP_setCaptureMode(ecapBase, ECAP_ONE_SHOT_CAPTURE_MODE, ECAP_EVENT_2);  // Only need 2 events
    ECAP_setEventPrescaler(ecapBase, 0);

    // Only 2 falling edges
    ECAP_setEventPolarity(ecapBase, ECAP_EVENT_1, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ecapBase, ECAP_EVENT_2, ECAP_EVNT_FALLING_EDGE);

    // Reset counter on each falling edge
    ECAP_enableCounterResetOnEvent(ecapBase, ECAP_EVENT_1);
    ECAP_enableCounterResetOnEvent(ecapBase, ECAP_EVENT_2);

    ECAP_enableTimeStampCapture(ecapBase);
    ECAP_reArm(ecapBase);
    ECAP_startCounter(ecapBase);

    // Enable interrupt when Event 2 hits
    ECAP_enableInterrupt(ecapBase, ECAP_ISR_SOURCE_CAPTURE_EVENT_2);

    if (ecapBase == ECAP1_BASE) {
        Interrupt_register(INT_ECAP1, &ecap1ISR);
        Interrupt_enable(INT_ECAP1);
    }
}
