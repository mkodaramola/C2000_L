#include "driverlib.h"
#include "device.h"
#include "mkod.h"


#define IN1 30
#define IN2 31
#define IN3 32
#define IN4 39


uint32_t in = 0;
uint32_t pot = 0;
uint32_t x = 0;

void Step(uint32_t, bool, uint32_t);
void StepHalf(uint32_t, bool, uint32_t);
void FullStep(uint32_t, bool, uint32_t);
void commute(bool a, bool b, bool c, bool d);



void main(void){

    Device_init();
    Device_initGPIO();

    GPIO_setPadConfig(IN1, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(IN1, GPIO_DIR_MODE_OUT);

    GPIO_setPadConfig(IN2, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(IN2, GPIO_DIR_MODE_OUT);

    GPIO_setPadConfig(IN3, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(IN3, GPIO_DIR_MODE_OUT);

    GPIO_setPadConfig(IN4, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(IN4, GPIO_DIR_MODE_OUT);

    Interrupt_initModule();
    Interrupt_initVectorTable();

    ADCA_init();




   while(1){


       pot = ADCAread(A0);


       StepHalf(360,1,3);


     // ESTOP0;


   }
}


void Step(uint32_t angle, bool dir, uint32_t d){



    uint32_t s = (angle * 2048UL)/360;

    for (in=0;in<s;in++){

        pot = ADCAread(A0);

        d = (pot*20)/4096;

        d = (d < 3) ? 3:d;
        x = d;

        if(dir){

          switch(in%4){

              case 0:
                  commute(1,0,0,0); break;
              case 1:
                  commute(0,1,0,0); break;
              case 2:
                  commute(0,0,1,0); break;
              case 3:
                  commute(0,0,0,1); break;
          }

        }

        else {

           switch(in%4){

           case 3:
               commute(1,0,0,0); break;
           case 2:
               commute(0,1,0,0); break;
           case 1:
               commute(0,0,1,0); break;
           case 0:
               commute(0,0,0,1); break;

                 } // endSwitch

            }  // else


            delay(d);

        } // __endFor



    }



void StepHalf(uint32_t angle, bool dir, uint32_t d){

    uint32_t steps = (angle * 4096UL)/360;

    for(in = 0; in < steps; in++) {

        pot = ADCAread(A0);
        d = (pot * 20) / 4096;
        d = (d < 3) ? 3 : d;
        x = d;

        uint8_t stepIndex = dir ? (in % 8) : (7 - (in % 8));

        switch(stepIndex){
            case 0: commute(1,0,0,0); break;
            case 1: commute(1,1,0,0); break;
            case 2: commute(0,1,0,0); break;
            case 3: commute(0,1,1,0); break;
            case 4: commute(0,0,1,0); break;
            case 5: commute(0,0,1,1); break;
            case 6: commute(0,0,0,1); break;
            case 7: commute(1,0,0,1); break;
        }

        delay(d);
    }
}






void commute(bool a, bool b, bool c, bool d){

    GPIO_writePin(IN1,a);
    GPIO_writePin(IN2,b);
    GPIO_writePin(IN3,c);
    GPIO_writePin(IN4,d);


}


void FullStep(uint32_t angle, bool dir, uint32_t d){


    uint32_t steps = (angle * 2048UL) / 360;  // Full-step: 512 steps/rev

    for (in = 0; in < steps; in++) {
        pot = ADCAread(A0);

        d = (pot*20)/4096;

        d = (d < 3) ? 3:d;
        x = d;
        switch (in % 4) {
            case 0: commute(1, 0, 1, 0); break;
            case 1: commute(0, 1, 1, 0); break;
            case 2: commute(0, 1, 0, 1); break;
            case 3: commute(1, 0, 0, 1); break;
        }

        if (!dir) {
            // Reverse steps
            switch (in % 4) {
                case 3: commute(1, 0, 1, 0); break;
                case 2: commute(0, 1, 1, 0); break;
                case 1: commute(0, 1, 0, 1); break;
                case 0: commute(1, 0, 0, 1); break;
            }
        }

        delay(d);
    }
}




