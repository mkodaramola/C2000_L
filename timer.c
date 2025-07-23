#include "driverlib.h"
#include "device.h"

unsigned long millis;
uint32_t sec = 0;



__interrupt void cpuTimer0ISR(void);
void initCPUTimers(void);
void configCPUTimer(uint32_t, float, float);

void main(void)
{
    Device_init();
    Interrupt_initModule();
    Interrupt_initVectorTable();

    // ISR for CPU Timer 0 interrupt
    Interrupt_register(INT_TIMER0, &cpuTimer0ISR);

    // Initialize CPU Timer
    initCPUTimers();

    // Configure CPU-Timer 0 to interrupt every 1 millisecond (1000ms)
    configCPUTimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, 1);

    CPUTimer_enableInterrupt(CPUTIMER0_BASE);

    // Enable CPU int1 which is connected to CPU-Timer 0
    // Enable TINT0 in the PIE: Group 1 interrupt 7
    Interrupt_enable(INT_TIMER0);

    // Start CPU-Timer 0
    CPUTimer_startTimer(CPUTIMER0_BASE);

    EINT;
    ERTM;
    unsigned long prev = 0;
    while(1)
    {

        if (millis - prev >= 1000) {
            sec++;
            prev = millis;
        }

    }
}


void initCPUTimers(void)
{
    // Set timer period to max
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);

    // Set prescaler to divide by 1
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);

    // Stop timer
    CPUTimer_stopTimer(CPUTIMER0_BASE);

    // Reload with period value
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);

    // Reset interrupt counter
    millis = 0;
}

//
// configCPUTimer - Configure selected timer
//
void configCPUTimer(uint32_t cpuTimer, float freq, float period)
{
    uint32_t temp;

    // Calculate period register value
    temp = (uint32_t)((freq / 1000) * period);
    CPUTimer_setPeriod(cpuTimer, temp - 1);

    CPUTimer_setPreScaler(cpuTimer, 0);
    CPUTimer_stopTimer(cpuTimer);
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(cpuTimer);

    // Reset interrupt counter
    if (cpuTimer == CPUTIMER0_BASE)
    {
        millis = 0;
    }
}

//
// cpuTimer0ISR - ISR for CPU Timer 0
//
__interrupt void cpuTimer0ISR(void)
{
    millis++;

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
