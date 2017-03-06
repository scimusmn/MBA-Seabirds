#include "robotZoneActuator.h"

///////////////////////////////////////////////////
//                  Declarations                 //
///////////////////////////////////////////////////

// declare teh linear actuator object. Arguments are as follows
//    actuator( direction pin,
//              speed pin,
//              analog input pin,
//              boolean value of whether or not to invert direction.


actuator left(7,6,2,true);
actuator right(5,3,1,true);

///////////////////////////////////////////////////
//              Configurable options             //
///////////////////////////////////////////////////


int delayOnOpen = 1000; //in milliseconds
int delayBetweenCycles = 1000;

///////////////////////////////////////////////////
// You should not need to edit below this point  //
///////////////////////////////////////////////////

int homeSensor = 16;
int outSensor = 17;
int visitorButton = 14;

bool home = true;

// if both of the rocks are done closing, set teh home flag.
void wait(){
  if(right.running == false && left.running == false){
    home = true;
    delay(delayBetweenCycles);
  }
}

void closeRocks(){
  if(right.running == false && left.running == false){
    while(digitalRead(visitorButton) == LOW);
    delay(delayOnOpen);
    Serial.println("Close the rocks");
    right.registerEndCB(wait);
    left.registerEndCB(wait);
    right.run(20);
    left.run(20);
  }
  
}

void openRocks(){
  Serial.println("Open the rocks");
  home = false;
  right.registerEndCB(closeRocks);
  left.registerEndCB(closeRocks);
  right.run(600);
  left.run(600);
}

//this is a bit of trickiness to get the rocks to finish their moves
// at the same time. It finds the difference between the percentages
// finished of each of the rocks, and then slows the speed of the 
// rock that is moving faster based on that percentage difference.

void trackTogether(){
  if(right.running && left.running){
    float pDiff = right.percentDone() - left.percentDone();
    //pDiff = max(-.5,min(.5,pDiff));
    int leftSpeed = constrain(255*(1 + pDiff*8),0,254);
    int rightSpeed = constrain(255*(1 - pDiff*8),0,254);
    Serial.println(pDiff,DEC);

    right.setSpeed(rightSpeed);
    left.setSpeed(leftSpeed);
  }
  
 // if(right.running) right.setSpeed(rightSpeed);
 // if(left.running) left.setSpeed(leftSpeed);
}

void setup() {
  //give ourselves some reprogramming time before
  //things start to move.
  delay(5000);

  pinMode(visitorButton,INPUT_PULLUP);

  Serial.begin(9600);
  closeRocks();
}

void loop() {
  //functions that handle all the nitty-gritty of moving the rocks
  left.idle();
  right.idle();

  if(home && digitalRead(visitorButton) == LOW){
    openRocks();
  }

  trackTogether();
}
