// C2000 version of the Arduino PING))) ultrasonic distance sensor demo
// Uses separate GPIOs for Trigger and Echo

#include "driverlib.h"
#include "device.h"

#define TRIGGER_PIN  10   // GPIO10 for trigger
#define ECHO_PIN     11   // GPIO11 for echo

volatile uint32_t startTime = 0;
volatile uint32_t endTime = 0;

uint32_t distance_cm =0;

void delayMicroseconds(uint32_t);
uint32_t pulseInEcho();
uint32_t microsecondsToCentimeters(uint32_t);
void initCPUTimer(void);



void main(void)
{
    Device_init();
    Device_initGPIO();

    GPIO_setPadConfig(TRIGGER_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(TRIGGER_PIN, GPIO_DIR_MODE_OUT);

    GPIO_setPadConfig(ECHO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(ECHO_PIN, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(ECHO_PIN, GPIO_QUAL_ASYNC);

    Interrupt_initModule();
    Interrupt_initVectorTable();
    initCPUTimer();

    for(;;)
    {
        // Send trigger pulse
        GPIO_writePin(TRIGGER_PIN, 0);
        delayMicroseconds(2);
        GPIO_writePin(TRIGGER_PIN, 1);
        delayMicroseconds(10);
        GPIO_writePin(TRIGGER_PIN, 0);

        // Measure echo pulse duration
        uint32_t duration = pulseInEcho();
         distance_cm = microsecondsToCentimeters(duration);

        // Breakpoint here to inspect distance_cm if needed
        DEVICE_DELAY_US(100000); // 100 ms delay
    }
}


void delayMicroseconds(uint32_t us)
{
    DEVICE_DELAY_US(us); // Accurate only if DEVICE_SYSCLK_FREQ is correct
}

// Returns pulse duration in microseconds
uint32_t pulseInEcho()
{
    // Wait for echo to go HIGH
    while(GPIO_readPin(ECHO_PIN) == 0);
    startTime = CPUTimer_getTimerCount(CPUTIMER0_BASE);

    // Wait for echo to go LOW
    while(GPIO_readPin(ECHO_PIN) == 1);
    endTime = CPUTimer_getTimerCount(CPUTIMER0_BASE);

    // Calculate elapsed time in CPU cycles and convert to microseconds
    uint32_t elapsedCycles = startTime - endTime;
    uint32_t us = (elapsedCycles * 1.0) / (DEVICE_SYSCLK_FREQ / 1000000);

    return us;
}

uint32_t microsecondsToCentimeters(uint32_t us)
{
    return us / 29 / 2;
}

void initCPUTimer(void)
{
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_startTimer(CPUTIMER0_BASE);
}
