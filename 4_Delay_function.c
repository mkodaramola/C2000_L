// d should be in millis
void delay(long double d){

    d *= 1000;

    DEVICE_DELAY_US(d);
}
