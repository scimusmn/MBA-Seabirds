#include "linearTrackActuators.h"

///////////////////////////////////////////////////
//                  Declarations                 //
///////////////////////////////////////////////////

//declare the actuator object. The arguments are as follows:

//   actuator ( pulseOutPin, directionControlPin, enablePin, leftLimitSwitch, rightLimitSwitch

actuator bird(3,5,6,16,15);

///////////////////////////////////////////////////
//              Configurable options             //
///////////////////////////////////////////////////

//time in seconds that the bird pauses at teh chicks
float holdTime = 1000; 

// maximum time that a bird can move before the program
// assumes that the limit switch is broken (in mS)
int runTimeout = 5000;


//movement profile for the journey outward.
// Default produces a forward motion with 
// slight periodic acceleration and deceleration.
//
// The oscillator period is 1.256 seconds, but
// with the 10 multiplier in the operand of the
// cosine, it has a period of .125 seconds. 
//
//To slow or speed the
// oscillation, simply add a multiplier to the operand of the 
//trig functions. 
//
//Units are inches per second.
float birdProfile (float osc){
  return 5 - cos(osc*8)*7.5;
}


///////////////////////////////////////////////////
// You should not need to edit below this point  //
///////////////////////////////////////////////////

// scale factor for inches to steps, measured empirically.
int stepsPerInch = 480;

//convert the inch measurements to step counts for the profiles.
signed int birdSteps (float osc){
  return birdProfile(osc)*stepsPerInch;
}

int visButton = 14;

bool home = true;

void sitAtHome(){
  home = true;
}

void holdAndBack(){
  while(digitalRead(visButton) == LOW);
  delay(holdTime);
  bird.registerEndCB(sitAtHome);
  bird.run(1);
}

void goOut(){
  home = false;
  bird.registerEndCB(holdAndBack);
  bird.run(0);
}

void setup() {

  bird.runTimeout = runTimeout;
  bird.setSpeedProfile(birdSteps,true);
  
  pinMode(visButton,INPUT_PULLUP);

  delay(5000);

  holdAndBack();
}

void loop() {
  bird.idle();
  
  if(digitalRead(14) == LOW && home) goOut();
}
