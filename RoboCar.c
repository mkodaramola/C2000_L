#include "driverlib.h"
#include "device.h"
#include "mkod.h"

#define TRIGGER_PIN  34
#define ECHO_PIN     43

volatile uint32_t startTime = 0;
volatile uint32_t endTime = 0;

float distance = 0, lastDis = 0;

unsigned long millis = 0;

__interrupt void cpuTimer1ISR(void);
void initCPUTimer(void);
void initCPUTimer1(void);
void configCPUTimer(uint32_t, float, float);

void delayMicroseconds(uint32_t);
uint32_t pulseInEcho();
uint32_t microsecondsToCentimeters(uint32_t);
uint32_t readDistance();
void BTcontrol();
float absv(float);

void compute(float);
void setGain(float,float,float);
void setSamplingTime(uint32_t);
void setLimit(float, float);


float speed = 0;


const uint32_t in1 = 1, in2 = 2, in3 = 3, in4 = 4, enA = 7, enB = 10;
const uint32_t sci_Tx = 8, sci_Rx = 9;


uint32_t samplingTime = 100;
float setpoint = 50;
float error = 0;
float kp = 0, ki = 0, kd = 0;
float P = 0, I = 0, D = 0;
float output = 0;
float lastInput = 0;

unsigned long lastMillis = 0;
float max, min;




void backward();
void forward();
void left();
void right();
void stop();


void main(void){
    Device_init();
    Device_initGPIO();

    GPIO_setPadConfig(TRIGGER_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(TRIGGER_PIN, GPIO_DIR_MODE_OUT);

    GPIO_setPadConfig(ECHO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(ECHO_PIN, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(ECHO_PIN, GPIO_QUAL_ASYNC);

    GPIO_setPadConfig(in1, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(in1, GPIO_DIR_MODE_OUT);

    GPIO_setPadConfig(in2, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(in2, GPIO_DIR_MODE_OUT);

    GPIO_setPadConfig(in3, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(in3, GPIO_DIR_MODE_OUT);


    GPIO_setPadConfig(in4, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(in4, GPIO_DIR_MODE_OUT);

    EPWM_t pwmA,pwmB;
    pwmA.init = initEPWM;
    pwmA.output = ePWM_out;

    pwmB.init = initEPWM;
    pwmB.output = ePWM_out;

    pwmA.init(&pwmA, enA);
    pwmB.init(&pwmB, enB);


    Interrupt_initModule();
    Interrupt_initVectorTable();

    initCPUTimer();

    Interrupt_register(INT_TIMER1, &cpuTimer1ISR);

    initCPUTimer1();
    configCPUTimer(CPUTIMER1_BASE, DEVICE_SYSCLK_FREQ, 1);
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    Interrupt_enable(INT_TIMER1);
    CPUTimer_startTimer(CPUTIMER1_BASE);



    sciA_init(sci_Tx, sci_Rx, 9600U);

    sciA_write("\r\nBluetooth SCI Ready\r\n");

    EINT;
    ERTM;

    setSamplingTime(100);
    setGain(1,1,1);
    setLimit(-115,115);



    while(1){

        distance = readDistance();




        compute(distance);


        pwmA.output(&pwmA, (uint16_t)speed);
        pwmB.output(&pwmB, (uint16_t)speed);

        if (error < 0) forward();

        else if (error > 0)backward();

        else stop();

        //BTcontrol();

    }//end_while


} // end_main








void compute(float input){


      float dt = ((float)samplingTime/1000);


       if (millis - lastMillis > samplingTime){

        error = setpoint - input;

        P = kp*error;

        I += (ki * error * dt);

        I = (I > max) ? max:((I < min) ? min:I);

        D = -((input - lastInput)/dt);

        output = P + I + D;

        //output = (output > max) ? max:((output < min) ? min:output);

        speed = absv(output) + 140;

        speed = (speed > 255) ? 255:((speed < 0) ? 0:speed);

        lastInput = input;

       lastMillis = millis;

       }

}

void setGain(float Kp, float Ki, float Kd){

    kp = Kp;
    ki = Ki;
    kd = Kd;

}

void setSamplingTime(uint32_t st){

    samplingTime = st;

}


void setLimit(float min_, float max_){

    min = min_;
    max = max_;
}





void initCPUTimer(void)
{
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_startTimer(CPUTIMER0_BASE);
}


void initCPUTimer1(void){
       CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);

       CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);

       CPUTimer_stopTimer(CPUTIMER1_BASE);

       CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
}

void configCPUTimer(uint32_t cpuTimer, float freq, float period){

    uint32_t temp;

    // Calculate period register value
    temp = (uint32_t)((freq / 1000) * period);
    CPUTimer_setPeriod(cpuTimer, temp - 1);

    CPUTimer_setPreScaler(cpuTimer, 0);
    CPUTimer_stopTimer(cpuTimer);
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(cpuTimer);

}


__interrupt void cpuTimer1ISR(void)
{

    millis++;

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}



void delayMicroseconds(uint32_t us){
    DEVICE_DELAY_US(us); // Accurate only if DEVICE_SYSCLK_FREQ is correct
}


uint32_t pulseInEcho(){
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

uint32_t microsecondsToCentimeters(uint32_t us){
    return us / 29 / 2;
}


uint32_t readDistance(){

    // Send trigger pulse
    GPIO_writePin(TRIGGER_PIN, 0);
    delayMicroseconds(2);
    GPIO_writePin(TRIGGER_PIN, 1);
    delayMicroseconds(10);
    GPIO_writePin(TRIGGER_PIN, 0);

    // Measure echo pulse duration
    uint32_t duration = pulseInEcho();
    uint32_t distance_cm = microsecondsToCentimeters(duration);

    // Breakpoint here to inspect distance_cm if needed
    DEVICE_DELAY_US(100000); // 100 ms delay

    return distance_cm;
}



void BTcontrol(){

    char c = sciA_read();

    if (c == 'f')
    {
        forward();



    }
    else if (c == 'b')
    {
        backward();

    }
    else if (c == 'r')
    {
        right();

    }
    else if (c == 'l')
    {
        left();

    }
    else if (c == 's')
    {
        stop();

    }


}


void stop(){

    GPIO_writePin(in1, 0);
    GPIO_writePin(in2, 0);
    GPIO_writePin(in3, 0);
    GPIO_writePin(in4, 0);

}


void forward(){

    GPIO_writePin(in1, 0);
    GPIO_writePin(in2, 1);
    GPIO_writePin(in3, 1);
    GPIO_writePin(in4, 0);

}

void backward(){


    GPIO_writePin(in1, 1);
    GPIO_writePin(in2, 0);
    GPIO_writePin(in3, 0);
    GPIO_writePin(in4, 1);

}

void left(){

    GPIO_writePin(in1, 0);
    GPIO_writePin(in2, 1);
    GPIO_writePin(in3, 0);
    GPIO_writePin(in4, 1);
}


void right(){

    GPIO_writePin(in1, 1);
    GPIO_writePin(in2, 0);
    GPIO_writePin(in3, 1);
    GPIO_writePin(in4, 0);

}


float absv(float n){

    if (n < 0) return (n*-1);
    else return n;

}


