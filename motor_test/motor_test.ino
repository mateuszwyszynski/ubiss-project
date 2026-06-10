int MOTOR_PIN_1 = 2;
int MOTOR_PIN_2 = 4;

// PWM motor pin
const int ledPin = 5;

void setup() {
  // put your setup code here, to run once:
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);

  pinMode(ledPin, OUTPUT);
}

void loop() {
  // // Spin in one direction
  // digitalWrite(MOTOR_PIN_1, HIGH);
  // digitalWrite(MOTOR_PIN_2, LOW);

  // delay(1000);

  // // Spin in the other direction
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, HIGH);

  // analogWrite(ledPin, 30);

  // delay(1000);

  // increase the LED brightness
  for(int dutyCycle = 0; dutyCycle <= 250; dutyCycle++){   
    // changing the LED brightness with PWM
    analogWrite(ledPin, dutyCycle);
    delay(100);
  }

  // digitalWrite(MOTOR_PIN_1, HIGH);
  // digitalWrite(MOTOR_PIN_2, LOW);

  // // decrease the LED brightness
  // for(int dutyCycle = 25; dutyCycle >= 0; dutyCycle--){
  //   // changing the LED brightness with PWM
  //   analogWrite(ledPin, 10*dutyCycle);
  //   delay(100);
  // }
}
