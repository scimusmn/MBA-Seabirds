#include "linearTrackActuators.h"

///////////////////////////////////////////////////
//                  Declarations                 //
///////////////////////////////////////////////////

//declare the actuator object. The arguments are as follows:

//   actuator ( pulseOutPin,
//              directionControlPin,
//              enablePin,
//              leftLimitSwitch,
//              rightLimitSwitch)

actuator bird(3,5,6,16,15);


///////////////////////////////////////////////////
//              Configurable options             //
///////////////////////////////////////////////////

// this is the distance which the bird will travel out to the
// left in the burrow. It is specified in inches
int distanceOut = 16; //in stepper counts

//time in seconds that the bird pauses at teh chicks
float holdTime = 1000; 

// maximum time that a bird can move before the program
// assumes that the limit switch is broken (in mS)
int runTimeout = 10000;

// movement profile for the return journey of the bird
// this is simply a flat speed with no wobbling. 
// Units are inches per second, and 24 is about the limit.
float returnProfile (float osc){
  //return 20;
  return 8 - cos(osc*8)*5;
}

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
float outProfile (float osc){
  return 8 - cos(osc*8)*5;
}

///////////////////////////////////////////////////
// You should not need to edit below this point  //
///////////////////////////////////////////////////

// pin which is used for visitor interaction.
int visButton = 14;

// controls the lighting in the burrow.
int lightPin = 9;

bool home = true;

// scale factor for inches to steps, measured empirically.
int stepsPerInch = 480;

//convert the inch measurements to step counts for the profiles.
signed int returnSteps (float osc){
  return returnProfile(osc)*stepsPerInch;
}

signed int outSteps (float osc){
  return outProfile(osc)*stepsPerInch;
}

//callback function for what to do when return to the home position
void sitAtHome(){
  // set the home flag.
  home = true;
}

// what to do once we reach the end position.
void outAndHold(){
  //sit until the button is released.
  while(digitalRead(visButton) == LOW);
  delay(holdTime);

  //set the speed profile to the return profile.
  bird.setSpeedProfile(returnSteps,true);

  //register what to do once we get home.
  bird.registerEndCB(sitAtHome);

  //run the bird back home.
  bird.run(1);
}

//function to begin the bird movement
void goOut(){
  // clear the home flag, locking out the visitor button.
  home = false;

  //change the speed profile.
  bird.setSpeedProfile(outSteps,true);

  // register the callback for the end of the move
  bird.registerEndCB(outAndHold);

  // send the bird out by distanceOut inches * steps per inch.
  bird.runByCounts(distanceOut*stepsPerInch,0);
}

void setup() {
  bird.runTimeout = runTimeout;
  
  pinMode(visButton,INPUT_PULLUP);

  delay(5000);

  outAndHold();
}

void loop() {
  bird.idle();

  // if the visitor button is press, and we're home,
  // send the bird out.
  if(digitalRead(visButton) == LOW && home) goOut();
}
