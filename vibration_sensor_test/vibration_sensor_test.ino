// Define the vibration sensor pin (matches the wiring diagram)
const int SENSOR_PIN = 18;

// Variables for vibration detection and debouncing
volatile bool vibrationDetected = false;
unsigned long lastTriggerTime = 0;
const unsigned long DEBOUNCE_DELAY = 250; // Delay in milliseconds to prevent double triggers

// Interrupt Service Routine (ISR) executed instantly when vibration closes the switch
void IRAM_ATTR sensorISR() {
  unsigned long currentTime = millis();
  
  // Check if enough time has passed since the last trigger
  if (currentTime - lastTriggerTime > DEBOUNCE_DELAY) {
    vibrationDetected = true;
    lastTriggerTime = currentTime;
  }
}

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  
  // Configure the pin with the internal pull-up resistor
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  
  // Attach the interrupt to the pin. Triggers when state goes HIGH -> LOW (FALLING)
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), sensorISR, FALLING);
  
  Serial.println("ESP32 initialized. Waiting for vibration...");
}

void loop() {
  // Check if the interrupt detected a vibration
  if (vibrationDetected) {
    Serial.println("Vibration detected!");
    
    // Reset the flag to allow catching the next vibration
    vibrationDetected = false;
  }
  
  // The loop can freely run other tasks here without delaying the sensor
}
