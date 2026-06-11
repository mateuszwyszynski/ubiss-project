#include <IRremote.h> // Library for handling IR signals
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;
 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
 
byte rowPins[ROWS] = {13, 12, 14, 27}; // Connect to R1-R4
byte colPins[COLS] = {26, 25, 33, 32}; // Connect to C1-C4
 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

#define VIBRATION_SENSOR_PIN 18
volatile bool vibrationDetected = false;
unsigned long lastTriggerTime = 0;
const unsigned long DEBOUNCE_DELAY = 250;



#define IR_RECV_PIN 15
#define LED_PIN 2
// Movement command enums
const int NO_COMMAND = 0;
const int FORWARD = 1;
const int BACWARD = 2;
const int TURN_LEFT = 3;
const int TURN_RIGHT = 4;
const int STEER_RIGHT = 6;
const int STEER_LEFT = 7;
const int STOP = 8;


const int FULLSPEED = 200;
const int TURNSPEED = 50;

int last_movement_command = NO_COMMAND;

#define MOTOR_IN1 4
#define MOTOR_IN2 22
#define MOTOR_IN3 21
#define MOTOR_IN4 19

// PWM motor pin
#define ENA 23
#define ENB 5// TODO: Check pin
void IRAM_ATTR sensorISR(){
  unsigned long currentTime = millis();

  if (currentTime - lastTriggerTime > DEBOUNCE_DELAY)
  {
    vibrationDetected = true;
    lastTriggerTime = currentTime;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);

  pinMode(LED_PIN, OUTPUT);

  IrReceiver.begin(IR_RECV_PIN, ENABLE_LED_FEEDBACK); // Start the receiver

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(VIBRATION_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(VIBRATION_SENSOR_PIN), sensorISR, FALLING);

  Serial.println("Setup done!");
}

// Interrupt Service Routine to detect when vibration happens


int parseCommand() {
  if (IrReceiver.decodedIRData.flags)
  {
    return last_movement_command;
  }

  switch (IrReceiver.decodedIRData.decodedRawData)
  {
    case 0xB946FF00:
    { 
      Serial.println("Forward");
      return FORWARD;
    }
    case 0xEA15FF00:
    { 
      Serial.println("Bacward");
      return BACWARD;
      break;
    }
    case 0xBB44FF00:
    { 
      Serial.println("Turn left");
      return TURN_LEFT;
      break;
    }
    case 0xBC43FF00:
    { 
      Serial.println("Turn right");
      return TURN_RIGHT;
      break;
    }
    case 0xBF40FF00:
    {
      Serial.println("Turn right");
      return STOP;
      break;
    }
    case 0xBA45FF00:
    {
      Serial.println("Turn left");
      return STEER_LEFT;
      break;
    }
    case 0xB847FF00:
    {
      Serial.println("Steer right");
      return STEER_RIGHT;
      break;
    }
    
    default:
    {
      Serial.println("Unregistered button");
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
      return NO_COMMAND;
    }
  }
  return NO_COMMAND;
}

int receiveCommand()
{
  if (IrReceiver.decode()) {
    // Print the received data in Hexadecimal
    int command = parseCommand();
    //Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    IrReceiver.resume();

    return command;
  }

  return NO_COMMAND;
}

void move(int speedA, int speedB)
{
  if (speedA > 0)
  {
    digitalWrite(MOTOR_IN2, LOW);
    digitalWrite(MOTOR_IN1, HIGH);
  }
  else
  {
    digitalWrite(MOTOR_IN2, HIGH);
    digitalWrite(MOTOR_IN1, LOW);
  }
  
  if (speedB > 0)
  {
    digitalWrite(MOTOR_IN4, LOW);
    digitalWrite(MOTOR_IN3, HIGH);
  }
  else
  {
    digitalWrite(MOTOR_IN4, HIGH);
    digitalWrite(MOTOR_IN3, LOW);
  }


  analogWrite(ENA, abs(speedA));
  analogWrite(ENB, abs(speedB));
}

void loop() {

  //digitalWrite(LED_PIN,  (millis() >> 8 ) &1);

  if(vibrationDetected) {
    Serial.println("Vibration detected");
    digitalWrite(LED_PIN, true);
    delay(50);
    digitalWrite(LED_PIN, false);
    delay(10);

    vibrationDetected = false;
  }

  char key = keypad.getKey();

  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);
  }


  int command = receiveCommand();

  switch (command)
  {
    case FORWARD:
    {
      move(FULLSPEED, FULLSPEED);
      break;
    }
    case BACWARD:
    {
      move(-FULLSPEED, -FULLSPEED);
      break;
    }
    case TURN_LEFT:
    {
      move(FULLSPEED/2, -FULLSPEED/2);
      break;
    }
    case TURN_RIGHT:
    {
      move(-FULLSPEED/2, FULLSPEED/2);
      break;
    }
    case STOP:
    {
      move(0, 0);
      break;
    }
    case STEER_RIGHT:
    {
      move(FULLSPEED - TURNSPEED, FULLSPEED );
      break;
    }
    case STEER_LEFT:
    {
      move(FULLSPEED , FULLSPEED - TURNSPEED);
      break;
    }
  }

  delay(10);
}
