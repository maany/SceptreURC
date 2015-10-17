#include <IRremote.h>

#define POWER_BUTTON 0x80FF48B7
#define TEMP_UP_BUTTON 0x80FF58A7
#define TEMP_DOWN_BUTTON 0x80FFC837
#define DEFAULT_ARRAY_SIZE 15

IRrecv irrecv(11); // Receive on pin 11
decode_results results;
int i = 0;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Setup complete");
}

void loop() {
  if (irrecv.decode(&results)) {
 //   Serial.println("Recieved");
    char pressedButton[10];
    decoder();
    irrecv.resume(); // Continue receiving
    
  }else {
  // Serial.println("Not received");
  }
  delay(1000);
}

void decoder() {
  switch(results.value){
    case POWER_BUTTON : Serial.println("POWER BUTTON"); break;
    case TEMP_UP_BUTTON : Serial.println("TEMP_UP BUTTON"); break;
    case TEMP_DOWN_BUTTON : Serial.println("TEMP_DOWN BUTTON"); break;
    default : Serial.println("OTHER BUTTON");
  }
}



