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
    Serial.println(results.value, HEX);
    char pressedButton[10];
    decoder(results.value,pressedButton);
    Serial.print("Detected Button : ");Serial.println(pressedButton);
    irrecv.resume(); // Continue receiving
    
  }else {
  // Serial.println("Not received");
  }
  delay(1000);
}

void decoder(long value,char* outVal) {
  Serial.println(value);
  if(value==POWER_BUTTON){
    outVal = "POWER" ; 
  }else if(value==TEMP_UP_BUTTON){
    outVal =  "TEMP UP";
  }else if(value=TEMP_DOWN_BUTTON){
    outVal =  "TEMP_DOWN";
  }
}



