#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "mkod.h"

uint32_t a = 0;
uint32_t b = 0;
uint32_t c = 0;

uint32_t d = 0;



void main(void)
{
    Device_init();
    Device_initGPIO();

    Board_init();

    EPWM_t epwm1 = {
        .init = initEPWM,
        .output = ePWM_out
    };

    EPWM_t epwm2;
    epwm2.init = initEPWM;
    epwm2.output = ePWM_out;

    EPWM_t epwm3;
    epwm3.init = initEPWM;
    epwm3.output = ePWM_out;

    SERVO_t servo;
    servo.S_init = servoInit;
    servo.S_write = servoWrite;

    epwm1.init(&epwm1, 1U);
    epwm2.init(&epwm2, 4U);
    epwm3.init(&epwm3, 7U);

    servo.S_init(&servo, 5);


    ADCA_init();

    while(1)
    {
        DEVICE_DELAY_US(1000);  // Throttle loop

        a = ADCAread(A2);        // Read 12-bit ADC value (0–4095)
        a = (a*255)/4096;
        b = ADCAread(A4);
        b = (b*255)/4096;
        c = ADCAread(A15);
        c = (c*255)/4096;

        d = ADCAread(A1);
        d = (d*180)/4096;

        servo.S_write(&servo, d);


        epwm1.output(&epwm1, a);    // Update PWM duty
        epwm2.output(&epwm2, b);
        epwm3.output(&epwm3, c);
    }
}
