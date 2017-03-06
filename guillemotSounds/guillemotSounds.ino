#include "sharpSensor.h"

///////////////////////////////////////////////////
//                  Declarations                 //
///////////////////////////////////////////////////

// declare the Sharp sensor object. Arguments are as follows:
// 
// Sensor ( analog pin number to which the signal is attached,
//          min value before a read is detected (0-255),
//          amount of time that the sensor averages after a high read

Sensor distance(0,60,500);

///////////////////////////////////////////////////
//              Configurable options             //
///////////////////////////////////////////////////

// max number of times that the audio will repeat while
// the sensor is continuously active
int maxRepeats = 3;

// time in between repeats in millisecond. Set to 0 to disable repeats
int repeatTime = 2000;

///////////////////////////////////////////////////
// You should not need to edit below this point  //
///////////////////////////////////////////////////

int audioTrigger = 7;
int audioSignal = 15;

bool playing = false;

// variables for handling repeats.
unsigned long repeatTimer = 0;
int repeatCount = 0;
bool logRepeat = false;

void onRead (int val){
  if(val > 100){
    playing = true;
    Serial.println("Playing audio");
    digitalWrite(audioTrigger,HIGH);
    delay(1000);
  }
}

void setup() {
  pinMode(audioTrigger,OUTPUT);
  pinMode(audioSignal,INPUT_PULLUP);
  
  distance.triggerCB = onRead;
  Serial.begin(9600);
  Serial.println("Starting up!");

  delay(5000);
}

void loop() {
  if(!playing) distance.idle();

  if(playing && digitalRead(audioSignal)){
    Serial.println("Audio finished");
    playing = false;

    if(repeatTime){
      repeatTimer = millis() + repeatTime;
      logRepeat = true;
    }
  }

  if(!distance.active()) repeatCount = 0;

  if(repeatTime && millis() > repeatTimer && logRepeat){
    logRepeat = false;
    if(distance.stillHigh){
      if(repeatCount < maxRepeats){
        repeatCount++;
        distance.reset();
      }
    }
      
  }
}
