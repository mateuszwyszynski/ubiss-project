#define MOTOR_IN1 4
#define MOTOR_IN2 22
#define MOTOR_IN3 21
#define MOTOR_IN4 19

// PWM motor pin
#define ENA 23
#define ENB 5

// PWM motor pin
const int ledPin = 5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(ledPin, OUTPUT);
}
void move(int value)
{
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);

  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, HIGH);

  analogWrite(ENA, value);
  analogWrite(ENB, value);
  Serial.println(value);
  delay(500);
}
void loop() {
  Serial.println("Test");
  // // Spin in one direction
  // digitalWrite(MOTOR_PIN_1, HIGH);
  // digitalWrite(MOTOR_PIN_2, LOW);

  // delay(1000);

  // // Spin in the other direction


  // analogWrite(ledPin, 30);

  // delay(1000);

  // increase the LED brightness
  for(int dutyCycle = 80 ; dutyCycle <= 130; dutyCycle += 2){   
    // changing the LED brightness with PWM
    move(dutyCycle);
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
