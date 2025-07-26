#define FREQ_HZ 50
#define HALF_PERIOD_US (1000000 / (FREQ_HZ * 2))  // 10ms = 10000us

const int pinA = 22;
const int pinB = 23;

TaskHandle_t inverterTaskHandle;

void inverterTask(void *parameter) {
  while (true) {
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, LOW);
    delayMicroseconds(HALF_PERIOD_US);

    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
    delayMicroseconds(HALF_PERIOD_US);
  }
}

void setup() {
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);

  xTaskCreatePinnedToCore(
    inverterTask,     // Task function
    "Inverter Task",  // Name
    1000,             // Stack size
    NULL,             // Parameters
    1,                // Priority
    &inverterTaskHandle, // Handle
    0                 // Core 0
  );
}

void loop() {
  // nothing here – all handled by Core 0
}
