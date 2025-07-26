#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "mkod.h"

#define EPWM_TIMER_TBPRD  (DEVICE_SYSCLK_FREQ / 2000)
#define EPWM_MAX          (EPWM_TIMER_TBPRD - 1)

#define EPWM_TIMER_TBPRD_SERVO  (DEVICE_SYSCLK_FREQ / 1600)
#define EPWM_MAX_SERVO          (EPWM_TIMER_TBPRD_SERVO - 1)


//-----------------------------
void initEPWM(EPWM_t* st_obj, const uint8_t GPIO_PIN) {



    uint32_t EPWM_PARAM[5];

    switch (GPIO_PIN) {
        case 0:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM1_BASE, GPIO_0_EPWM1_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
        case 1:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM1_BASE, GPIO_1_EPWM1_B, EPWM_COUNTER_COMPARE_B, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB}, sizeof(EPWM_PARAM)); break;
        case 2:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM2_BASE, GPIO_2_EPWM2_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
        case 3:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM2_BASE, GPIO_3_EPWM2_B, EPWM_COUNTER_COMPARE_B, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB}, sizeof(EPWM_PARAM)); break;
        case 4:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM3_BASE, GPIO_4_EPWM3_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
        case 5:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM3_BASE, GPIO_5_EPWM3_B, EPWM_COUNTER_COMPARE_B, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB}, sizeof(EPWM_PARAM)); break;
        case 6:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM4_BASE, GPIO_6_EPWM4_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
        case 7:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM4_BASE, GPIO_7_EPWM4_B, EPWM_COUNTER_COMPARE_B, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB}, sizeof(EPWM_PARAM)); break;
        case 8:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM5_BASE, GPIO_8_EPWM5_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
        case 9:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM5_BASE, GPIO_9_EPWM5_B, EPWM_COUNTER_COMPARE_B, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB}, sizeof(EPWM_PARAM)); break;
        case 10: memcpy(EPWM_PARAM, (uint32_t[]){EPWM6_BASE, GPIO_10_EPWM6_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
        case 11: memcpy(EPWM_PARAM, (uint32_t[]){EPWM6_BASE, GPIO_11_EPWM6_B, EPWM_COUNTER_COMPARE_B, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB}, sizeof(EPWM_PARAM)); break;
        default: memcpy(EPWM_PARAM, (uint32_t[]){EPWM1_BASE, GPIO_0_EPWM1_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
    }

    st_obj->base = EPWM_PARAM[0];
    st_obj->compareReg = EPWM_PARAM[2];

    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);


    // Add this before using the ePWM module
    switch (EPWM_PARAM[0]) {
        case EPWM1_BASE: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM1); break;
        case EPWM2_BASE: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM2); break;
        case EPWM3_BASE: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM3); break;
        case EPWM4_BASE: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM4); break;
        case EPWM5_BASE: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM5); break;
        case EPWM6_BASE: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM6); break;
        default: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM7); break;
    }


    GPIO_setPinConfig(EPWM_PARAM[1]);
    GPIO_setPadConfig(GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_PIN, GPIO_QUAL_SYNC);

    EPWM_setTimeBasePeriod(st_obj->base, EPWM_TIMER_TBPRD);
    EPWM_setCounterCompareValue(st_obj->base, (EPWM_CounterCompareModule)st_obj->compareReg, EPWM_MAX);
    EPWM_setTimeBaseCounterMode(st_obj->base, EPWM_COUNTER_MODE_UP);
    EPWM_setClockPrescaler(st_obj->base, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);

    EPWM_setActionQualifierAction(st_obj->base, (EPWM_ActionQualifierOutputModule)EPWM_PARAM[3], EPWM_AQ_OUTPUT_HIGH, (EPWM_ActionQualifierOutputEvent)EPWM_PARAM[4]);
    EPWM_setActionQualifierAction(st_obj->base, (EPWM_ActionQualifierOutputModule)EPWM_PARAM[3], EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);


}

//----
void ePWM_out(EPWM_t* st_obj, uint16_t pwmVal) {
    pwmVal = (pwmVal * EPWM_MAX)/255;
    pwmVal = EPWM_MAX - pwmVal;
    EPWM_setCounterCompareValue(st_obj->base, (EPWM_CounterCompareModule)st_obj->compareReg, pwmVal);
}


// ------------------------------
void servoInit(SERVO_t* st_obj, const uint8_t GPIO_PIN) {

    uint32_t EPWM_PARAM[5];

    switch (GPIO_PIN) {
        case 0:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM1_BASE, GPIO_0_EPWM1_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
        case 1:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM1_BASE, GPIO_1_EPWM1_B, EPWM_COUNTER_COMPARE_B, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB}, sizeof(EPWM_PARAM)); break;
        case 2:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM2_BASE, GPIO_2_EPWM2_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
        case 3:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM2_BASE, GPIO_3_EPWM2_B, EPWM_COUNTER_COMPARE_B, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB}, sizeof(EPWM_PARAM)); break;
        case 4:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM3_BASE, GPIO_4_EPWM3_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
        case 5:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM3_BASE, GPIO_5_EPWM3_B, EPWM_COUNTER_COMPARE_B, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB}, sizeof(EPWM_PARAM)); break;
        case 6:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM4_BASE, GPIO_6_EPWM4_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
        case 7:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM4_BASE, GPIO_7_EPWM4_B, EPWM_COUNTER_COMPARE_B, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB}, sizeof(EPWM_PARAM)); break;
        case 8:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM5_BASE, GPIO_8_EPWM5_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
        case 9:  memcpy(EPWM_PARAM, (uint32_t[]){EPWM5_BASE, GPIO_9_EPWM5_B, EPWM_COUNTER_COMPARE_B, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB}, sizeof(EPWM_PARAM)); break;
        case 10: memcpy(EPWM_PARAM, (uint32_t[]){EPWM6_BASE, GPIO_10_EPWM6_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
        case 11: memcpy(EPWM_PARAM, (uint32_t[]){EPWM6_BASE, GPIO_11_EPWM6_B, EPWM_COUNTER_COMPARE_B, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB}, sizeof(EPWM_PARAM)); break;
        default: memcpy(EPWM_PARAM, (uint32_t[]){EPWM1_BASE, GPIO_0_EPWM1_A, EPWM_COUNTER_COMPARE_A, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA}, sizeof(EPWM_PARAM)); break;
    }

    st_obj->base = EPWM_PARAM[0];
    st_obj->compareReg = EPWM_PARAM[2];

    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);


    // Add this before using the ePWM module
    switch (EPWM_PARAM[0]) {
        case EPWM1_BASE: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM1); break;
        case EPWM2_BASE: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM2); break;
        case EPWM3_BASE: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM3); break;
        case EPWM4_BASE: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM4); break;
        case EPWM5_BASE: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM5); break;
        case EPWM6_BASE: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM6); break;
        default: SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM7); break;
    }


    GPIO_setPinConfig(EPWM_PARAM[1]);
    GPIO_setPadConfig(GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_PIN, GPIO_QUAL_SYNC);

    EPWM_setTimeBasePeriod(st_obj->base, EPWM_TIMER_TBPRD_SERVO);
    EPWM_setCounterCompareValue(st_obj->base, (EPWM_CounterCompareModule)st_obj->compareReg, EPWM_MAX_SERVO);
    EPWM_setTimeBaseCounterMode(st_obj->base, EPWM_COUNTER_MODE_UP);
    EPWM_setClockPrescaler(st_obj->base, EPWM_CLOCK_DIVIDER_4, EPWM_HSCLOCK_DIVIDER_8);

    EPWM_setActionQualifierAction(st_obj->base, (EPWM_ActionQualifierOutputModule)EPWM_PARAM[3], EPWM_AQ_OUTPUT_HIGH, (EPWM_ActionQualifierOutputEvent)EPWM_PARAM[4]);
    EPWM_setActionQualifierAction(st_obj->base, (EPWM_ActionQualifierOutputModule)EPWM_PARAM[3], EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);


}


//----
void servoWrite(SERVO_t* servo, uint8_t angle){

    if(angle > 180) angle = 180;

    uint32_t min = ((5 * EPWM_MAX_SERVO)/100);

    uint32_t diff = ((10 * EPWM_MAX_SERVO)/100) - min;

    diff *= 2;

    diff = (angle*diff)/180;

    diff += (min/2);

    diff = EPWM_MAX_SERVO - diff;
    EPWM_setCounterCompareValue(servo->base, (EPWM_CounterCompareModule)servo->compareReg, diff);


}


//-----------------------------------------




void ADCA_init(){

   ADC_setVREF(ADCA_BASE, ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);
   ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_2_0);
   ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
   ADC_enableConverter(ADCA_BASE);
   DEVICE_DELAY_US(5000);

}


uint16_t ADCAread(ADCA_CH ch) {

    uint16_t av = 0;

    switch (ch) {
               case A0: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0, 8U); break;
               case A1: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN1, 8U); break;
               case A2: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2, 8U); break;
               case A3: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN3, 8U); break;
               case A4: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN4, 8U); break;
               case A5: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN5, 8U); break;
               case A6: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN6, 8U); break;
               case A7: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN7, 8U); break;
               case A8: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN8, 8U); break;
               case A9: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN9, 8U); break;
               case A10: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN10, 8U); break;
               case A11: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN11, 8U); break;
               case A12: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN12, 8U); break;
               case A13: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN13, 8U); break;
               case A14: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN14, 8U); break;
               case A15: ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN15, 8U); break;
               default: return 0;
           }


    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);

    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);
    while (!ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1)) {}
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    av = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);

    return av;
}

//------------------------------ UART

void pinConfig(uint32_t p){

    switch (p){
    case 2: GPIO_setPinConfig(GPIO_2_SCIA_TX); break;
    case 8: GPIO_setPinConfig(GPIO_8_SCIA_TX); break;
    case 16: GPIO_setPinConfig(GPIO_16_SCIA_TX); break;
    case 24: GPIO_setPinConfig(GPIO_24_SCIA_TX); break;
    case 29: GPIO_setPinConfig(GPIO_29_SCIA_TX); break;
    case 37: GPIO_setPinConfig(GPIO_37_SCIA_TX); break;
    case 3: GPIO_setPinConfig(GPIO_3_SCIA_RX); break;
    case 17: GPIO_setPinConfig(GPIO_17_SCIA_RX); break;
    case 25: GPIO_setPinConfig(GPIO_25_SCIA_RX); break;
    case 28: GPIO_setPinConfig(GPIO_28_SCIA_RX); break;
    case 35: GPIO_setPinConfig(GPIO_35_SCIA_RX); break;
    case 9: GPIO_setPinConfig(GPIO_9_SCIA_RX); break;
    default: ESTOP0;

    }

}

void sciA_init(uint32_t TxPin, uint32_t RxPin, uint32_t baud){


       // SCIA RX
       pinConfig(RxPin);
       GPIO_setDirectionMode(RxPin, GPIO_DIR_MODE_IN);
       GPIO_setPadConfig(RxPin, GPIO_PIN_TYPE_STD);
       GPIO_setQualificationMode(RxPin, GPIO_QUAL_ASYNC);


       // SCIA TX
       pinConfig(TxPin);
       GPIO_setDirectionMode(TxPin, GPIO_DIR_MODE_OUT);
       GPIO_setPadConfig(TxPin, GPIO_PIN_TYPE_STD);
       GPIO_setQualificationMode(TxPin, GPIO_QUAL_ASYNC);

       // SCI Configuration
       SCI_performSoftwareReset(SCIA_BASE);
       SCI_setConfig(SCIA_BASE, DEVICE_LSPCLK_FREQ, baud,
                     (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
       SCI_resetChannels(SCIA_BASE);
       SCI_resetRxFIFO(SCIA_BASE);
       SCI_resetTxFIFO(SCIA_BASE);
       SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
       SCI_enableFIFO(SCIA_BASE);
       SCI_enableModule(SCIA_BASE);
       SCI_performSoftwareReset(SCIA_BASE);

#ifdef AUTOBAUD
   SCI_lockAutobaud(SCIA_BASE);
#endif


}


void sciA_write(unsigned char *msg){

    SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, strlen(msg));
}


char sciA_read(){

       uint16_t receivedChar;
       uint16_t rxStatus;

       receivedChar = SCI_readCharBlockingFIFO(SCIA_BASE);

       rxStatus = SCI_getRxStatus(SCIA_BASE);
       if ((rxStatus & SCI_RXSTATUS_ERROR) != 0)
       {
           ESTOP0;
       }

       return (char)(receivedChar & 0xFF);

}




void delay(long d){

    d *= 1000;

    DEVICE_DELAY_US(d);
}

void delayMs_t0(uint32_t ms)
{
    CPUTimer_setEmulationMode(CPUTIMER0_BASE, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_stopTimer(CPUTIMER0_BASE);

    uint32_t ticks = (DEVICE_SYSCLK_FREQ / 1000) * ms;

    CPUTimer_setPeriod(CPUTIMER0_BASE, ticks);
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_clearOverflowFlag(CPUTIMER0_BASE);
    CPUTimer_startTimer(CPUTIMER0_BASE);

    while (!CPUTimer_getTimerOverflowStatus(CPUTIMER0_BASE));
    CPUTimer_stopTimer(CPUTIMER0_BASE);
}


void delayMs_t1(uint32_t ms)
{
    CPUTimer_setEmulationMode(CPUTIMER1_BASE, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);
    CPUTimer_stopTimer(CPUTIMER1_BASE);

    uint32_t ticks = (DEVICE_SYSCLK_FREQ / 1000) * ms;

    CPUTimer_setPeriod(CPUTIMER1_BASE, ticks);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_clearOverflowFlag(CPUTIMER1_BASE);
    CPUTimer_startTimer(CPUTIMER1_BASE);

    while (!CPUTimer_getTimerOverflowStatus(CPUTIMER1_BASE));
    CPUTimer_stopTimer(CPUTIMER1_BASE);
}



void delayMs_t2(uint32_t ms)
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