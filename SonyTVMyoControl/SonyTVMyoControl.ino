/*
* IRrecord: record and play back IR signals as a minimal
* An IR detector/demodulator must be connected to the input RECV_PIN.
* An IR LED must be connected to the output PWM pin 3.
* A button must be connected to the input BUTTON_PIN; this is the
* send button.
* A visible LED can be connected to STATUS_PIN to provide status.
*
* The logic is:
* If the button is pressed, send the IR code.
* If an IR code is received, record it.
*
* Version 0.11 September, 2009
* Copyright 2009 Ken Shirriff
* http://arcfn.com
*/

#include <IRremote.h>
#include <MyoController.h>
int RECV_PIN = 11;
int BUTTON_PIN = 7;
int STATUS_PIN = 13;

IRrecv irrecv(RECV_PIN);
IRsend irsend;

decode_results results;
MyoController myo = MyoController();

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  pinMode(BUTTON_PIN, INPUT);
  pinMode(STATUS_PIN, OUTPUT);

  myo.initMyo();
}

// Storage for the recorded code
int codeType = -1; // The type of code
unsigned long codeValue; // The code value if not raw
unsigned int rawCodes[RAWBUF]; // The durations if raw
int codeLen; // The length of the code
int toggle = 0; // The RC5/6 toggle state
unsigned long codeValues[5]; // store codeValue mapped to myo gestures by index
        // Stores the code for later playback
        // Most of this code is just logging
void storeCode(decode_results *results) {
  codeType = results->decode_type;
  int count = results->rawlen;
  if (codeType == UNKNOWN) {
    Serial.println("Received unknown code, saving as raw");
    codeLen = results->rawlen - 1;
    // To store raw codes:
    // Drop first value (gap)
    // Convert from ticks to microseconds
    // Tweak marks shorter, and spaces longer to cancel out IR receiver distortion
    for (int i = 1; i <= codeLen; i++) {
      if (i % 2) {
        // Mark
        rawCodes[i - 1] = results->rawbuf[i] * USECPERTICK - MARK_EXCESS;
        Serial.print(" m");
      }
      else {
        // Space
        rawCodes[i - 1] = results->rawbuf[i] * USECPERTICK + MARK_EXCESS;
        Serial.print(" s");
      }
      Serial.print(rawCodes[i - 1], DEC);
    }
    Serial.println("");
  }
  else {
    if (codeType == NEC) {
      Serial.print("Received NEC: ");
      if (results->value == REPEAT) {
        // Don't record a NEC repeat value as that's useless.
        Serial.println("repeat; ignoring.");
        return;
      }
    }
    else if (codeType == SONY) {
      Serial.print("Received SONY: ");
    }
    else if (codeType == RC5) {
      Serial.print("Received RC5: ");
    }
    else if (codeType == RC6) {
      Serial.print("Received RC6: ");
    }
    else {
      Serial.print("Unexpected codeType ");
      Serial.print(codeType, DEC);
      Serial.println("");
    }
    Serial.println(results->value, HEX);
    codeValue = results->value;
    codeLen = results->bits;
  }
}

void sendCode(int repeat) {
  if (codeType == NEC) {
    if (repeat) {
      irsend.sendNEC(REPEAT, codeLen);
      Serial.println("Sent NEC repeat");
    }
    else {
      irsend.sendNEC(codeValue, codeLen);
      Serial.print("Sent NEC ");
      Serial.println(codeValue, HEX);
    }
  }
  else if (codeType == SONY) {
    irsend.sendSony(codeValue, codeLen);
    Serial.print("Sent Sony ");
    Serial.println(codeValue, HEX);
  }
  else if (codeType == RC5 || codeType == RC6) {
    if (!repeat) {
      // Flip the toggle bit for a new button press
      toggle = 1 - toggle;
    }
    // Put the toggle bit into the code to send
    codeValue = codeValue & ~(1 << (codeLen - 1));
    codeValue = codeValue | (toggle << (codeLen - 1));
    if (codeType == RC5) {
      Serial.print("Sent RC5 ");
      Serial.println(codeValue, HEX);
      irsend.sendRC5(codeValue, codeLen);
    }
    else {
      irsend.sendRC6(codeValue, codeLen);
      Serial.print("Sent RC6 ");
      Serial.println(codeValue, HEX);
    }
  }
  else if (codeType == UNKNOWN /* i.e. raw */) {
    // Assume 38 KHz
    irsend.sendRaw(rawCodes, codeLen, 38);
    Serial.println("Sent raw");
  }
}

int lastButtonState;

void loop() {
  delay(1000);
  int buttonState = digitalRead(BUTTON_PIN);
  
  // When signal sent and it just turns low, init reciever.
  if (lastButtonState == HIGH && buttonState == LOW) {
    Serial.println("Released");
    irrecv.enableIRIn(); // Re-enable receiver
  }
  // Send Signal
  if (buttonState) {
    Serial.println("Pressed, sending");
    digitalWrite(STATUS_PIN, HIGH);
    // set code on basis of myo gesture
    Serial.println("Give Myo Gesture after 2 seconds");
    
    setCodeFromMyo();
    sendCode(lastButtonState == buttonState);
    digitalWrite(STATUS_PIN, LOW);
    delay(50); // Wait a bit between retransmissions
  } // receive signal and decode
  else if (irrecv.decode(&results)) {
    Serial.println("receiving");
    digitalWrite(STATUS_PIN, HIGH);
    storeCode(&results);
    int i;
    for (i = 0; i < 5;i++){
      if (codeValues[i] == '\0') {
        codeValues[i] = codeValue;
        break;
      }
    }
    if (i == 5) {
      Serial.println("Myo code values array full. Switch to send mode");
    }
    irrecv.resume(); // resume receiver
    digitalWrite(STATUS_PIN, LOW);
  }
  lastButtonState = buttonState;

}
//=====================================
// Code to control Sending data via myo
// array position 0 : fist
// array position 1 : waveIn
// array position 2 : waveOut
// array position 3 : fingerSpread
// array position 4 : doubleTap
//=====================================
void setCodeFromMyo() {
  myo.updatePose();
  switch (myo.getCurrentPose()) {
  case rest:
    Serial.println("Myo at rest");
    codeValue = 0x000000;
    break;
  case fist:
    Serial.println("Myo Gesture : Fist");
    codeValue = codeValues[0];
    break;
  case waveIn:
    Serial.println("Myo Gesture : WaveIn");
    codeValue = codeValues[1];
    break;
  case waveOut:
    Serial.println("Myo Gesture : WaveOut");
    codeValue = codeValues[2];
    break;
  case fingersSpread:
    Serial.println("Myo Gesture : FingerSpread");
    codeValue = codeValues[3];
    break;
  case doubleTap:
    Serial.println("Myo Gesture : DoubleTap");
    codeValue = codeValues[4];
    break;
  }
}
