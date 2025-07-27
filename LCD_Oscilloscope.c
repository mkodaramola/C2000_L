#include "driverlib.h"
#include "device.h"
#include <string.h>
#include "mkod.h"

#define SER_PIN     22U
#define SRCLK_PIN   41U
#define RCLK_PIN    37U
#define HALL1_GPIO              8   // GPIO8

volatile float period_ms = 0;
volatile uint32_t capEvent1 = 0;
volatile uint32_t capEvent2 = 0;

volatile uint32_t c = 0;
float PP = 1000.0f/(float)DEVICE_SYSCLK_FREQ;
volatile uint8_t data = 0;
 uint8_t pdata = 0;
 uint32_t adc = 0;

__interrupt void ecap1ISR(void);
__interrupt void cpuTimer0ISR(void);

//-----

void shiftOut_LSB_First(uint8_t);

void lcd_data(unsigned char);
void lcd_cmd(unsigned char);
void lcd_init();
void lcd_string(const char*);
void setCursor(bool,uint8_t);
void lcd_clear();

void configCPUTimer(uint32_t, float, float);
void setupECAP(uint32_t, uint32_t, uint32_t, XBAR_InputNum);


void lcd_createCustomChar(uint8_t location, uint8_t charmap[]) {
    location &= 0x7; // max 8 locations (0-7)
    lcd_cmd(0x40 | (location << 3)); // set CGRAM address
    int i = 0;
    for (i = 0; i < 8; i++) {
        lcd_data(charmap[i]);
    }
}

uint8_t  man[8] = {

 0b00100,
 0b01010,
 0b00100,
 0b01110,
 0b10101,
 0b00100,
 0b01010

  };


uint8_t  sqw100[8] = {

 0b11111,
 0b10001,
 0b10001,
 0b10001,
 0b10001,
 0b10001,
 0b10001

  };


uint8_t  sqw80[8] = {

 0b11110,
 0b10010,
 0b10010,
 0b10010,
 0b10010,
 0b10010,
 0b10011

  };

uint8_t  sqw60[8] = {

 0b11100,
 0b10100,
 0b10100,
 0b10100,
 0b10100,
 0b10100,
 0b10111

  };

uint8_t  sqw40[8] = {

 0b11000,
 0b11000,
 0b11000,
 0b11000,
 0b11000,
 0b11000,
 0b11111

  };


uint8_t  sqw20[8] = {

 0b10000,
 0b10000,
 0b10000,
 0b10000,
 0b10000,
 0b10000,
 0b11111

  };

uint8_t  sqw0[8] = {

 0b00000,
 0b00000,
 0b00000,
 0b00000,
 0b00000,
 0b00000,
 0b11111

  };






uint8_t  bullet[8] = {

 0b00011,
 0b00111,
 0b01111,
 0b11111,
 0b11111,
 0b01111,
 0b00111,
 0b00011

  };

uint8_t  Introbullet[8] = {

 0b11000,
 0b11100,
 0b11110,
 0b11111,
 0b11111,
 0b11110,
 0b11100,
 0b11000

  };



void main(void)
{
    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();

    configCPUTimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, 500);
    Interrupt_register(INT_TIMER0, &cpuTimer0ISR);
    Interrupt_enable(INT_TIMER0);


    setupECAP(ECAP1_BASE, HALL1_GPIO, GPIO_8_GPIO8, XBAR_INPUT7);

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

    ADCA_init();

    lcd_init();

    lcd_createCustomChar(0, sqw100);
    lcd_createCustomChar(1, sqw80);
    lcd_createCustomChar(2, sqw60);
    lcd_createCustomChar(3, sqw40);
    lcd_createCustomChar(4, sqw20);
    lcd_createCustomChar(5, sqw0);

    EINT;
    ERTM;

    lcd_clear();
   setCursor(0, 0);
   lcd_data(2);
   setCursor(1, 0);
   lcd_string("60%");

    while (1) {

        int i;
        adc = ADCAread(A0);

        adc = (adc * 100)/4095;

        if(pdata < (adc-5) ||pdata > (adc+5)){


            if (adc >= 95){

               lcd_clear();

               for (i=0;i<16;i++){
                   setCursor(0, i);
                   lcd_data(0);
               }

                setCursor(1, 0);
                lcd_string("100%");

            }


            if(adc >= 80 && adc <= 94){

            lcd_clear();
            for (i=0;i<16;i++){
                setCursor(0, i);
                lcd_data(1);
            }

             setCursor(1, 0);
             lcd_string("80%");

          }

            if(adc >= 60 && adc <= 79){

            lcd_clear();
            for (i=0;i<16;i++){
                setCursor(0, i);
                lcd_data(2);
            }

             setCursor(1, 0);
             lcd_string("60%");
          }

            if(adc >= 40 && adc <= 59){

            lcd_clear();
            for (i=0;i<16;i++){
                setCursor(0, i);
                lcd_data(3);
            }

             setCursor(1, 0);
             lcd_string("40%");
          }

            if(adc >= 20 && adc <= 39){

            lcd_clear();
            for (i=0;i<16;i++){
                setCursor(0, i);
                lcd_data(4);
            }

             setCursor(1, 0);
             lcd_string("20%");
          }


            if(adc >= 0 && adc <= 19){

            lcd_clear();
            for (i=0;i<16;i++){
                setCursor(0, i);
                lcd_data(5);
            }

             setCursor(1, 0);
             lcd_string("0%");
          }


            pdata = adc;
        }








    }
}








__interrupt void ecap1ISR(void)
{

    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);

    capEvent1 = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_1);
    capEvent2 = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_2);


    period_ms = (float)capEvent2 * PP;

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



void configCPUTimer(uint32_t base, float cpuFreqHz, float periodMs)
{
    CPUTimer_setPeriod(base, 0xFFFFFFFF);
    CPUTimer_setPreScaler(base, 0);
    CPUTimer_stopTimer(base);
    CPUTimer_reloadTimerCounter(base);


    uint32_t periodCount = (uint32_t)((cpuFreqHz / 1000.0f) * periodMs);
    CPUTimer_setPeriod(base, periodCount - 1);
    CPUTimer_setPreScaler(base, 0);
    CPUTimer_stopTimer(base);
    CPUTimer_reloadTimerCounter(base);
    CPUTimer_setEmulationMode(base, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(base);

    CPUTimer_startTimer(base);

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


void shiftOut_LSB_First(uint8_t data) {
    uint8_t i = 0;
    for (i = 0; i < 8; i++) {
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

void lcd_data(unsigned char data) {
    uint8_t MSB = (data & 0xF0) | 0x0A;
    uint8_t LSB = (data << 4) | 0x0A;

    shiftOut_LSB_First(MSB);
    DEVICE_DELAY_US(1);
    shiftOut_LSB_First(MSB & ~(1 << 1));
    DEVICE_DELAY_US(40);

    shiftOut_LSB_First(LSB);
    DEVICE_DELAY_US(1);
    shiftOut_LSB_First(LSB & ~(1 << 1));
    DEVICE_DELAY_US(40);
}

void lcd_cmd(unsigned char cmd) {
    uint8_t MSB = (cmd & 0xF0) | 0x02;
    uint8_t LSB = (cmd << 4) | 0x02;

    shiftOut_LSB_First(MSB);
    DEVICE_DELAY_US(1);
    shiftOut_LSB_First(MSB & ~(1 << 1));
    DEVICE_DELAY_US(40);

    shiftOut_LSB_First(LSB);
    DEVICE_DELAY_US(1);
    shiftOut_LSB_First(LSB & ~(1 << 1));

    if (cmd == 0x01 || cmd == 0x02)
        DEVICE_DELAY_US(2000);
    else
        DEVICE_DELAY_US(40);
}

void lcd_init() {
    DEVICE_DELAY_US(40000);

    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);

    DEVICE_DELAY_US(20000);
}

void lcd_string(const char *str) {
    uint16_t len = strlen(str);
    uint16_t i = 0;
    for (i = 0; i < len; i++) {
        lcd_data((unsigned char)str[i]);
    }
}

void setCursor(bool r, uint8_t c) {
    uint8_t r0 = 0x80 + c; // Line 0 address
    uint8_t r1 = 0xC0 + c; // Line 1 address

    if (r)
        lcd_cmd(r1);
    else
        lcd_cmd(r0);
}

void lcd_clear() {
    lcd_cmd(0x01);
    lcd_cmd(0x0C);
}


