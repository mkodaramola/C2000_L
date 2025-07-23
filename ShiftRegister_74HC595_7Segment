#include "driverlib.h"
#include "device.h"


#define ZERO 252
#define ONE 96
#define TWO 218
#define THREE 242
#define FOUR 102
#define FIVE 182
#define SIX 190
#define SEVEN 224
#define EIGHT 254
#define NINE 246
#define A 238
#define a 238
#define B 62
#define b 62
#define C 156
#define c 156
#define D 122
#define d 122
#define E 158
#define e 158
#define F 142
#define f 142
#define DOT 1

#define SER_PIN     22U
#define SRCLK_PIN   41U
#define RCLK_PIN    37U



void shiftOut_LSB_First(uint8_t);

void delayMs_t2_(uint32_t ms)
{
    CPUTimer_setEmulationMode(CPUTIMER2_BASE, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0);
    CPUTimer_stopTimer(CPUTIMER2_BASE);

    uint32_t ticks = (DEVICE_SYSCLK_FREQ / 1000) * ms;

    CPUTimer_setPeriod(CPUTIMER2_BASE, ticks);
    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);
    CPUTimer_clearOverflowFlag(CPUTIMER2_BASE);
    CPUTimer_startTimer(CPUTIMER2_BASE);

    while(!CPUTimer_getTimerOverflowStatus(CPUTIMER2_BASE));
    CPUTimer_stopTimer(CPUTIMER2_BASE);
}



int ch[] = {
    ZERO, ONE, TWO, THREE, FOUR, FIVE,
    SIX, SEVEN, EIGHT, NINE, A,
    B, C, D, E, F
};

void main(void){
    Device_init();
    Device_initGPIO();

    Interrupt_initModule();
    Interrupt_initVectorTable();



    GPIO_setPinConfig(GPIO_22_GPIO22); // For SER
    GPIO_setPinConfig(GPIO_41_GPIO41); // For SRCLK
    GPIO_setPinConfig(GPIO_37_GPIO37); // For RCLK


   GPIO_setPadConfig(SER_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(SER_PIN, GPIO_DIR_MODE_OUT);

   GPIO_setPadConfig(SRCLK_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(SRCLK_PIN, GPIO_DIR_MODE_OUT);

   GPIO_setPadConfig(RCLK_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(RCLK_PIN, GPIO_DIR_MODE_OUT);

    GPIO_writePin(SER_PIN, 0);
    GPIO_writePin(SRCLK_PIN, 0);
    GPIO_writePin(RCLK_PIN, 0);




    while (1)
    {
        uint8_t j =0;
        for(j = 0; j < 16; j++){
            shiftOut_LSB_First(ch[j]);
            delayMs_t2_(1000);
            shiftOut_LSB_First(DOT);
            delayMs_t2_(1000);

        }




    }

}



void shiftOut_LSB_First(uint8_t data) {

    int i =0;
    for (i= 0; i < 8; i++) {
        GPIO_writePin(SER_PIN, (data >> i) & 0x01);

        GPIO_writePin(SRCLK_PIN, 1);
        DEVICE_DELAY_US(1);
        GPIO_writePin(SRCLK_PIN, 0);
        DEVICE_DELAY_US(1);
    }

    GPIO_writePin(RCLK_PIN, 1);
    DEVICE_DELAY_US(1);
    GPIO_writePin(RCLK_PIN, 0);
    DEVICE_DELAY_US(1);


}





