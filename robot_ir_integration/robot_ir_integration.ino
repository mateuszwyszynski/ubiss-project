#include <IRremote.h> // Library for handling IR signals
#include <Keypad.h>
#include <Adafruit_NeoPixel.h>
//#include <DHT.h>
//#define DHT11_PIN  16 // ESP32 pin GPIO21 connected to DHT11 sensor
//DHT dht11(DHT11_PIN, DHT11);


#define sgn(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0)) // From: https://forum.arduino.cc/t/sgn-sign-signum-function-suggestions/602445

struct MovementCommand{
  int MotorA; // from 0 - 100 %
  int MotorB; // from 0 - 100 %
};

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
char charBuffer[11] = "";
int charIndex = 0;
unsigned long character_inputted = 0;

#define VIBRATION_SENSOR_PIN 18
volatile bool vibrationDetected = false;
unsigned long lastTriggerTime = 0;
const unsigned long DEBOUNCE_DELAY = 250;

int vibariton_cont_for_scare = 10;



#define IR_RECV_PIN 15
#define LED_PIN 2
#define LED_COUNT 8
Adafruit_NeoPixel ring = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void fillRing(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < LED_COUNT; i++) ring.setPixelColor(i, ring.Color(r, g, b));
  ring.show();
}
// Movement command enums
const int NO_COMMAND = 0;
const int FORWARD = 1;
const int BACWARD = 2;
const int TURN_LEFT = 3;
const int TURN_RIGHT = 4;
const int STEER_RIGHT = 6;
const int STEER_LEFT = 7;
const int STOP = 8;
const int SCARE = 9;
const int UNSCARE = 10;



const float SCARED_MODIFIER = 0.7;
const int MAXSPEED = 200;
const int MINSPEED = 100;
const int TURNSPEED = 20;
const int MA_OFFSET = 0;
const int MB_OFFSET = 30;

int vibration_count = 0;

MovementCommand FORWARD_COMMAND{100, 100};
MovementCommand BACKWARD_COMMAND{-100, -100};
MovementCommand TURN_RIGHT_COMMAND{-100/2, 100/2};
MovementCommand TURN_LEFT_COMMAND{100/2, -100/2};
MovementCommand STEER_RIGHT_COMMAND{100 - TURNSPEED, 100 };
MovementCommand STEER_LEFT_COMMAND{100, 100 - TURNSPEED};
MovementCommand STOP_COMMAND{0, 0};

//float FREQUENCY = 0.008;
float FREQUENCY = 0.008;
int AMPLITUDE = 60;

bool scared = false;
bool previousScared = false;
bool robotMoving = false;


int last_movement_command = NO_COMMAND;

unsigned int scared_start = 0;
//int scared_timeout = 10 * 1000;

MovementCommand lastRawCommand{0, 0};

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

  ring.begin();
  ring.setBrightness(60);
  fillRing(0, 255, 0);  // start "safe"

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
      Serial.println("Stop");
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
    case 0xB54AFF00:
    {
      Serial.println("Scared");
      return SCARE;
      break;
    }
    case 0xBD42FF00:
    {
      Serial.println("Unscared");
      return UNSCARE;
      break;
    }
    

    
    default:
    {
      int command = IrReceiver.decodedIRData.decodedRawData;
      if (command == 0)
        return NO_COMMAND;
      Serial.println("Unregistered button");
      Serial.println(command, HEX);
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
void performCommand(int command)
{
  MovementCommand mov_command = {0,0};
  switch (command)
  {
    case FORWARD:
    {
      mov_command = FORWARD_COMMAND;
      break;
    }
    case BACWARD:
    {
      mov_command = BACKWARD_COMMAND;
      break;
    }
    case TURN_LEFT:
    {
      mov_command = TURN_LEFT_COMMAND;
      break;
    }
    case TURN_RIGHT:
    {
      mov_command = TURN_RIGHT_COMMAND;
      break;
    }
    case STOP:
    {
      mov_command = STOP_COMMAND;
      break;
    }
    case STEER_RIGHT:
    {
      mov_command = STEER_RIGHT_COMMAND;
      break;
    }
    case STEER_LEFT:
    {
      mov_command = STEER_LEFT_COMMAND;
      break;
    }
    case SCARE:
    {
      scare();
      return;
    }
    case UNSCARE:
      unscare();
      return;
    default:
      return;
  }
  Serial.println("Movement_command");
  move(mov_command, true);
}

void move(MovementCommand command, bool updateLastCommand)
{
  if (updateLastCommand)
  {
    lastRawCommand = command;
  }
  int speedA = command.MotorA;
  int speedB = command.MotorB;

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
  int commandA = abs(speedA);
  int commandB = abs(speedB);
  if (speedA == 0 && speedB == 0)
  {
    Serial.println("Zero velocity command");
    robotMoving = false;
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);

    // Serial.print("PWM_A:");
    // Serial.println(0);

    // Serial.print("PWM_B:");
    // Serial.println(0);
    return;
  }
  else
  {
    robotMoving = true;
  }
  constrain(commandA, 0, 100);
  constrain(commandB, 0, 100);
  
  int pwmA = map(commandA, 0, 100, MINSPEED, MAXSPEED);
  int pwmB = map(commandB, 0, 100, MINSPEED, MAXSPEED);

  pwmA -= MA_OFFSET;
  pwmB -= MB_OFFSET;


  // Serial.print("PWM: (");
  // Serial.print(pwmA);
  // Serial.print(",");
  // Serial.print(pwmB);
  // Serial.println(")");
  // Serial.println(scared);
  analogWrite(ENA, pwmA);
  analogWrite(ENB, pwmB);



}

void scare()
{
  scared = true;
  scared_start = millis();
  Serial.print("Scare. Commnads: ");
  Serial.print(lastRawCommand.MotorA);
  Serial.print(" ");
  Serial.println(lastRawCommand.MotorB);
  fillRing(255, 255, 255);
  move(lastRawCommand, true);

  
}

void unscare()
{
  scared = false;
  scared_start = 0;
  vibration_count = 0;
  fillRing(0, 255, 0);
  move(STOP_COMMAND, true);
}
MovementCommand stagger()
{
  if (lastRawCommand.MotorA == 0 and lastRawCommand.MotorB == 0)
    return STOP_COMMAND;
  // Serial.print("Raw command: (");
  // Serial.print(lastRawCommand.MotorA);
  // Serial.print(",");
  // Serial.print(lastRawCommand.MotorB);
  // Serial.print(") ");

  float time = millis() * FREQUENCY;
  int sway = sin(time) * AMPLITUDE;
  float slowDown = 0.2;

  MovementCommand staggerCommand = {lastRawCommand.MotorA, lastRawCommand.MotorB};

  // Scale speed down
  staggerCommand.MotorA = int(staggerCommand.MotorA * slowDown );
  staggerCommand.MotorB = int(staggerCommand.MotorB * slowDown );

  // Serial.print("Speed: (");
  // Serial.print(staggerCommand.MotorA);
  // Serial.print(",");
  // Serial.print(staggerCommand.MotorB);
  // Serial.print(") ");


  // Add sway
  staggerCommand.MotorA = constrain(abs(staggerCommand.MotorA) + sway, 0, 100);
  staggerCommand.MotorB = constrain(abs(staggerCommand.MotorB) - sway, 0, 100);

  // Preserve sign of original command
  staggerCommand.MotorA = sgn(lastRawCommand.MotorA) * staggerCommand.MotorA;
  staggerCommand.MotorB = sgn(lastRawCommand.MotorB) * staggerCommand.MotorB;



  
  return staggerCommand;
}
void loop() {

  //digitalWrite(LED_PIN,  (millis() >> 8 ) &1);
  // static unsigned long lastPrintTime = 0;
  // if(millis() - lastPrintTime > 1000)
  // {
  //   float humi  = dht11.readHumidity();
  //   Serial.print("Humidity: ");
  //   Serial.println(humi);
  //   lastPrintTime = millis();
  // }
  //digitalWrite(LED_PIN,  scared);

  // if ((millis() >> 7 ) &1)
  // {
  //   Serial.print("Detected vibrations: ");
  //   Serial.println(vibration_count);
  // }
  // float humi  = dht11.readHumidity();
  // if (scared && humi >= 60)
  // {
  //   Serial.println(humi);
  //   unscare();
  // }
  if (scared)
  {
    if(robotMoving)
    {
      move(stagger(), false);
    }
  }

  if (vibrationDetected)
  {
    vibration_count++;
    vibrationDetected = false;
    Serial.println("Vibration detected!");
  }

  if(vibration_count > vibariton_cont_for_scare && !scared) {
    scare();
    vibration_count = 0;
  }

  char key = keypad.getKey();

  if(key)
  {
    character_inputted = millis();
    if (charIndex < 10){
      charBuffer[charIndex++] = key;
      charBuffer[charIndex] = '\0';
      Serial.println(key);
    }
    else 
    {
      charBuffer[0] = '\0';
      charIndex = 0;
    }
    if (charBuffer[0] == '#' && charIndex > 2 && charBuffer[charIndex - 1] == '*')
    {
      charBuffer[charIndex - 1] = '\0';  // remove the '*'
      int value = atoi(&charBuffer[1]);   // parse digits between '#' and '*'
      Serial.print("Parsed: ");
      Serial.println(value);
      //scared_timeout = value * 1000;
      vibariton_cont_for_scare = value;
      charBuffer[0] = '\0';
      charIndex = 0;
    }
  }


  if (character_inputted > 0 && millis() - character_inputted > 3000)
  {
    charBuffer[0] = '\0';
    charIndex = 0;
    Serial.println("Buffer reseted");
    character_inputted = 0;
  }

  int command = receiveCommand();
  performCommand(command);
  delay(10);

  previousScared = scared;

}
