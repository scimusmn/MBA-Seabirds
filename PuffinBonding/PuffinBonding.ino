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

// Set the delay times at the end of the jump, specified in milliseconds
int endDelay = 3000;

// Set the speed of the puffins movement. This is a value from 0 to 255, 255 being full speed.
int bondingSpeed = 255;

///////////////////////////////////////////////////
// You should not need to edit below this point  //
///////////////////////////////////////////////////
int puffinMotor = 11;
int playerTrigger = 3;
int playerSignal = 16;

bool bonding = false;

void onAudioStop(){
  bonding = false;
  analogWrite(puffinMotor,0);
}

void startBonding (){
  bonding = true;
  digitalWrite(playerTrigger,HIGH);
  analogWrite(puffinMotor,bondingSpeed);
  delay(100);
  digitalWrite(playerTrigger,LOW);
  delay(1000);
}

// setup function, which is run only once at the program start.
void setup() {
  Serial.begin(9600);
  pinMode(puffinMotor,OUTPUT);
  digitalWrite(puffinMotor,LOW);
  pinMode(playerTrigger,OUTPUT);
  pinMode(playerSignal,INPUT_PULLUP);

  crank.triggerCB = startBonding;

  delay(5000);
  //startHopping();
}

// The main loop, that is running constantly.
void loop() {
  //if we are not bonding
  if(!bonding){
    // watch the rotary switch
    crank.idle();
  }

  if(bonding && digitalRead(playerSignal)){
    onAudioStop();
  }
}
