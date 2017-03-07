#include "rotaryControl.h"

///////////////////////////////////////////////////
//                  Declarations                 //
///////////////////////////////////////////////////

// declare the rotary control object. Arguments are as follows

//    rotaryControl( pin number for input ,
//                   number of counts before triggering (default is 30),
//                   timeout before resetting counts (default is 1000 ms)){

rotaryControl crank(14,12,1000);

///////////////////////////////////////////////////
//              Configurable options             //
///////////////////////////////////////////////////

// Time that the visitor has to crank before triggering.
// If set to any number besides zero, visitor still has to crank
// a minimum of the counts specified above, but they also
// have to be cranking for a minimum of minTriggerTime milliseconds.

int minTriggerTime = 2000;

// Set the speed of the puffins movement. This is a value from 0 to 255, 255 being full speed.
int bondingSpeed = 255;

// set the range of the on and off times while bonding.
// Times are given in milliseconds. While running,
// the birds will turn on for a random time between minOnTime
// and maxOnTime, and then turn off for a random amount of time
// between minOffTime and maxOffTime
int minOnTime = 500;
int maxOnTime = 1500;
int minOffTime = 250;
int maxOffTime = 750;

///////////////////////////////////////////////////
// You should not need to edit below this point  //
///////////////////////////////////////////////////

// declare pins for the wiring
int puffinMotor = 11;
int playerTrigger = 3;
int playerSignal = 16;

bool bonding = false;

// Function to call once the audio stops playing.
void onAudioStop(){
  bonding = false;
  analogWrite(puffinMotor,0);
}

// function to call once the rotary crank triggers
void startBonding (){
  // set the bonding flag.
  bonding = true;

  // press the play button until we see confirmation that we're playing
  while(digitalRead(playerSignal)){
    digitalWrite(playerTrigger,HIGH);
    delay(50);
    digitalWrite(playerTrigger,LOW);
    delay(50);
  }

  // write the motor to the speed specified above.
  analogWrite(puffinMotor,bondingSpeed);
}

// setup function, which is run only once at the program start.
void setup() {
  Serial.begin(9600);

  // init the GPIOs
  pinMode(puffinMotor,OUTPUT);
  digitalWrite(puffinMotor,LOW);
  pinMode(playerTrigger,OUTPUT);
  pinMode(playerSignal,INPUT_PULLUP);

  //set the trigger callback.
  crank.triggerCB = startBonding;

  crank.triggerTime = minTriggerTime;

  delay(5000);
}

unsigned long onTimer = 0;
unsigned long offTimer = 0;
bool motorOn;

// The main loop, that is running constantly.
void loop() {
  //if we are not bonding
  if(!bonding){
    // watch the rotary switch
    crank.idle();
  }

  // code to control the randomized on and off of the motor
  if(bonding && offTimer < millis() && !motorOn){
    // if we're off, and the off timer has elapsed, 
    // set the on timer, and run the motor.
    onTimer = millis() + random(minOnTime,maxOnTime);
    analogWrite(puffinMotor, bondingSpeed);
    motorOn = true;
  } else if (bonding && onTimer < millis() && motorOn){
    // if we're on, and the on timer has elapsed, 
    // set the off timer, and stop the motor.
    offTimer = millis() + random(minOffTime,maxOffTime);
    analogWrite(puffinMotor, 0);
    motorOn = false;
  }

  // once the audio playing relay open (audio has stopped) 
  // do the audio stop callback.
  if(bonding && digitalRead(playerSignal)){
    onAudioStop();
  }
}
