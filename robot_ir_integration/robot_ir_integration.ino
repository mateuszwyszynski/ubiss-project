#include <IRremote.h> // Library for handling IR signals

#define IR_RECV_PIN 15
// Movement command enums
const int NO_COMMAND = 0;
const int FORWARD = 1;
const int BACWARD = 2;
const int TURN_LEFT = 3;
const int TURN_RIGHT = 4;

int last_movement_command = NO_COMMAND;

#define MOTOR_IN1 2
#define MOTOR_IN2 4
#define MOTOR_IN3 14
#define MOTOR_IN4 12

// PWM motor pin
#define ENA 5
#define ENB 6// TODO: Check pin

void setup() {
  // put your setup code here, to run once:
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
   pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);

  IrReceiver.begin(IR_RECV_PIN, ENABLE_LED_FEEDBACK); // Start the receiver

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
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
    default:
    {
      Serial.println("Unregistered button");
      return NO_COMMAND;
    }
  }
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


  analogWrite(ENA, speedA);
  analogWrite(ENB, speedB);
}

void loop() {

  int command = receiveCommand();

  switch (command)
  {
    case FORWARD:
    {
      move(255, 255);
      break;
    }
    case BACWARD:
    {
      move(-255, -255);
      break;
    }
    case TURN_LEFT:
    {
      move(-255, 255);
      break;
    }
    case TURN_RIGHT:
    {
      move(255, -255);
      break;
    }
  }
  
}
