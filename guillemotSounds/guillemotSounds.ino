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

// function which handles a read from the sensor
void onRead (int val){
  if(val > 100){
    // if the read is greater than 100, start playing
    playing = true;
    Serial.println("Playing audio");
    // press the play button until we see confirmation that we're playing
    while(digitalRead(playerSignal)){
      digitalWrite(playerTrigger,HIGH);
      delay(50);
      digitalWrite(playerTrigger,LOW);
      delay(50);
    }
  }
}

void setup() {
  // init GPIOs
  pinMode(audioTrigger,OUTPUT);
  pinMode(audioSignal,INPUT_PULLUP);
  
  distance.triggerCB = onRead;
  Serial.begin(9600);
  Serial.println("Starting up!");

  delay(5000);
}

void loop() {
  // if we aren't playing, watch the sensor
  if(!playing) distance.idle();

  // if we are playing, and the relay indicates the audio stopped,
  if(playing && digitalRead(audioSignal)){
    Serial.println("Audio finished");
    // clear the playing flag
    playing = false;

    // if we have a repeat time set
    if(repeatTime){
      // start the timer, and set the repeat flag. 
      repeatTimer = millis() + repeatTime;
      logRepeat = true;
    }
  }

  // if the sensor has dropped below the trigger threshold,
  // reset the repeat counter
  if(!distance.active()) repeatCount = 0;

  // if the repeat timer has just elapsed
  if(repeatTime && millis() > repeatTimer && logRepeat){
    // clear the flag so we don't do this twice.
    logRepeat = false;

    // and if the sensor is still reading high,
    // and we've repeated less than maxRepeats
    if(distance.stillHigh){
      if(repeatCount < maxRepeats){
        //increment the repeat counter
        repeatCount++;
        // and reenable the sensor.
        distance.reenable();
      }
    }
      
  }
}
