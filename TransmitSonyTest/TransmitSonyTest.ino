#include <IRremote.h>

#define POWER_BUTTON 0xA90
#define CHANNEL_UP_BUTTON 0x90
#define CHANNEL_DOWN_BUTTON 0x890
#define DEFAULT_ARRAY_SIZE 15

IRrecv irrecv(11); // Receive on pin 11
IRsend irsend;
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
    
    char pressedButton[10];
    //decoder();
    irrecv.resume(); // Continue receiving
    
  }else {
  // Serial.println("Not received");
  }
  sendPowerButton();
  delay(5000);
}

void decoder() {
  
  Serial.println(results.value,HEX);
  Serial.println(results.decode_type);
  switch(results.value){
    case POWER_BUTTON : Serial.println("POWER BUTTON"); sendPowerButton();break;
    case CHANNEL_UP_BUTTON : Serial.println("CHANNEL_UP BUTTON"); sendChannelUp();break;
    case CHANNEL_DOWN_BUTTON : Serial.println("CHANNEL_DOWN BUTTON"); sendChannelDown();break;
    default : Serial.println("OTHER BUTTON");
  }
}
  void sendPowerButton(){
   irsend.sendSony(POWER_BUTTON,12);
   /* for(int i=0;i<3;i++){
      irsend.sendSony(POWER_BUTTON,12);
      delay(100);
    }
    */
  }
  void sendChannelUp(){
    irsend.sendSony(CHANNEL_UP_BUTTON,12);
    /*     for(int i=0;i<3;i++){
      irsend.sendSony(CHANNEL_UP_BUTTON,12);
      delay(100);
    }
    */
  }
  void sendChannelDown(){
    irsend.sendSony(CHANNEL_DOWN_BUTTON,12);
     /*
     for(int i=0;i<3;i++){
      irsend.sendSony(CHANNEL_DOWN_BUTTON,12);
      delay(100);
   
    }*/
  }




