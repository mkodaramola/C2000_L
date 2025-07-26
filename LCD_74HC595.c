#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "c2000ware_libraries.h"
#include <string.h>

#define SER_PIN     22U
#define SRCLK_PIN   41U
#define RCLK_PIN    37U

void shiftOut_LSB_First(uint8_t);
void lcd_data(unsigned char);
void lcd_cmd(unsigned char);
void lcd_init();
void lcd_string(const char*);
void setCursor(bool, uint8_t);
void lcd_clear();

void main(void)
{
    Device_init();
    Device_initGPIO();

    // GPIO configuration for 74HC595
    GPIO_setPinConfig(GPIO_22_GPIO22); // SER
    GPIO_setPinConfig(GPIO_41_GPIO41); // SRCLK
    GPIO_setPinConfig(GPIO_37_GPIO37); // RCLK

    GPIO_setPadConfig(SER_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(SER_PIN, GPIO_DIR_MODE_OUT);

    GPIO_setPadConfig(SRCLK_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(SRCLK_PIN, GPIO_DIR_MODE_OUT);

    GPIO_setPadConfig(RCLK_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(RCLK_PIN, GPIO_DIR_MODE_OUT);

    GPIO_writePin(SER_PIN, 0);
    GPIO_writePin(SRCLK_PIN, 0);
    GPIO_writePin(RCLK_PIN, 0);

    lcd_init();




    while (1){


        uint8_t i;
        for (i=0;i<16;i++){

            setCursor(0, i);
            lcd_string("Henry");

            setCursor(1, 16-i);
            lcd_string("Femi");



            DEVICE_DELAY_US(500000);
            lcd_clear();
        }



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
