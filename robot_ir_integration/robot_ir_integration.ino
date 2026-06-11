#include <IRremote.h> // Library for handling IR signals

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

#define MOTOR_IN1 12
#define MOTOR_IN2 14
#define MOTOR_IN3 32
#define MOTOR_IN4 33

// PWM motor pin
#define ENA 25
#define ENB 5// TODO: Check pin

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

  Serial.println("Setup done!");
}

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
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
  }
  else
  {
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
  }
  
  if (speedB > 0)
  {
    digitalWrite(MOTOR_IN3, LOW);
    digitalWrite(MOTOR_IN4, HIGH);
  }
  else
  {
    digitalWrite(MOTOR_IN3, HIGH);
    digitalWrite(MOTOR_IN4, LOW);
  }


  analogWrite(ENA, abs(speedA));
  analogWrite(ENB, abs(speedB));
}

void loop() {

  digitalWrite(LED_PIN,  (millis() >> 8 ) &1);
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
      move(-FULLSPEED/2, FULLSPEED/2);
      break;
    }
    case TURN_RIGHT:
    {
      move(FULLSPEED/2, -FULLSPEED/2);
      break;
    }
    case STOP:
    {
      move(0, 0);
      break;
    }
    case STEER_RIGHT:
    {
      move(FULLSPEED, FULLSPEED - TURNSPEED);
      break;
    }
    case STEER_LEFT:
    {
      move(FULLSPEED - TURNSPEED, FULLSPEED);
      break;
    }
  }

  delay(10);
}
