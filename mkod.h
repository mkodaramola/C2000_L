#ifndef MKOD_H
#define MKOD_H

#include <stdint.h>


// ePWM
typedef struct EPWM_t EPWM_t;

struct EPWM_t {
    uint32_t base;
    uint16_t compareReg;
    void (*init)(EPWM_t*, uint8_t);
    void (*output)(EPWM_t*, uint16_t);
};


void initEPWM(EPWM_t*, uint8_t);
void ePWM_out(EPWM_t*, uint16_t);


// Servo
typedef struct SERVO_t SERVO_t;

struct SERVO_t{

    uint32_t base;
    uint16_t compareReg;
    void (*S_init)(SERVO_t*, uint8_t);
    void (*S_write)(SERVO_t*, uint16_t);

};


void servoInit(SERVO_t*,uint8_t);
void servoWrite(SERVO_t*, uint16_t);


//ADC
typedef enum ADCA_CH ADCA_CH;
enum ADCA_CH {
    A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15
};


void ADCA_init();

uint16_t ADCAread(ADCA_CH);


//UART

void sciA_init(uint32_t, uint32_t, uint32_t);
void sciA_write(unsigned char*);
char sciA_read();




//Delay
void delay(long milliseconds);

#endif
