//#include "Freenove_IR_Lib_for_ESP32.h"
#define LED_PIN 2
#include <IRremote.h> // Library for handling IR signals

const int RECV_PIN = 15; // Define the Signal pin to GPIO 15
const int NO_COMMAND = 0;
const int FORWARD = 1;
const int BACWARD = 2;
const int TURN_LEFT = 3;
const int TURN_RIGHT = 4;

int last_movement_command = NO_COMMAND;


//Freenove_ESP32_IR_Recv ir_recv(15); //ESP32-GPIO15

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
  pinMode(LED_PIN, OUTPUT);
  // pinMode(15, INPUT);
  Serial.println("Setup done!");
}
int parseCommand() {
  if (IrReceiver.decodedIRData.flags)
  {
    return last_movement_command;
  }
 
  
  switch (IrReceiver.decodedIRData.decodedRawData)
  {
    case 0x123456:
    { 
      Serial.println("Forward");
      last_movement_command = FORWARD;
      break;
    }
    case 0x123457:
    { 
      Serial.println("Bacward");
      return BACWARD;
      break;
    }
    case 0x123458:
    { 
      Serial.println("Turn left");
      return TURN_LEFT;
      break;
    }
    case 0x123459:
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
void loop() {
  //digitalWrite(LED_PIN,  (millis() >> 8 ) &1);
  //digitalWrite(LED_PIN,  digitalRead(15));

  // bool value = 
  if (IrReceiver.decode()) {
    // Print the received data in Hexadecimal
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    IrReceiver.resume(); // Enable receiving of the next value

    // last_movement_command = parseCommand()
  }
}

  

  //Serial.println("LED ON");
  // ir_recv.task();
  // if(ir_recv.nec_available())
  // {

  //   digitalWrite(LED_PIN, true);
  //   Serial.printf("IR Protocol:%s, IR Code: %#x\r\n", ir_recv.protocol(), ir_recv.data());
  //   //Serial.println("LED ON");
  //}
