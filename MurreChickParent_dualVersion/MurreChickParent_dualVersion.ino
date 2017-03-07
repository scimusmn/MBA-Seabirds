#include "linearTrackActuators.h"

///////////////////////////////////////////////////
//                  Declarations                 //
///////////////////////////////////////////////////

//declare the actuator objects. The arguments are as follows:

//   actuator ( pulseOutPin, directionControlPin, enablePin, leftLimitSwitch, rightLimitSwitch )

actuator parent(9,7,10,17,15);
actuator chick(2,4,5,14,16);

///////////////////////////////////////////////////
//              Configurable options             //
///////////////////////////////////////////////////

// Set the delay times at the end of travel to the sides, specified in milliseconds
int endDelay = 1000;

// The distance from center offset of each of the birds in the home position, in inches.
// Positive Value is to the left, negative is to the right
float chickCenter = 0;
float parentCenter = -0.5;

// Movement profile for the parent. Mostly flat, with a little wobble. 
// Units are inches per second, and 24 is about the limit.
float parentProfile (float osc){
  return 6.5 - cos(osc*4)*1.5;
}

// this profile returns a perfectly flat velocity of 7 inches per second
/*float parentProfile (float osc){
  return 7;
}*/

// Movement profile for the chick.
// Default produces a forward motion with 
// slight periodic acceleration and deceleration.
//
// The oscillator period is 1.256 seconds, but
// with the 8 multiplier in the operand of the
// cosine, it has a period of .157 seconds. 
//
//To slow or speed the
// oscillation, simply add a multiplier to the operand of the 
//trig functions. Multiply by >1 to increase the speed, <1 to decrease.
//
//Units are inches per second.
signed int chickProfile (float osc){
  return 5 - cos(osc*8)*10;
}

///////////////////////////////////////////////////
// You should not need to edit below this point  //
///////////////////////////////////////////////////

int rightVisSwitch = 18;
int leftVisSwitch = 12;

// Stores the total width of travel of the birds, in inches:
float totalTravel = 6.25;

// scale factor for inches to steps, measured empirically.
int stepsPerInch = 500;

//convert the inch measurements to step counts for the profiles.
signed int parentSteps (float osc){
  return parentProfile(osc)*stepsPerInch;
}

signed int chickSteps (float osc){
  return chickProfile(osc)*stepsPerInch;
}

bool home = false;

// what happens when the parent reaches the left side
void pLeftSide(){
  // tell the chick what to do next
  chick.registerEndCB(centerLeft);

  //hold while the button is still pressed.
  while(digitalRead(12) == LOW);

  //delay by the amount specified above
  delay(endDelay);

  // if the joystick was pressed in the other direction, send to the other side.
  // Otherwise, return to the center. 
  if(digitalRead(18) == LOW) goRight();
  else chick.runByCounts((totalTravel/2 - chickCenter)*stepsPerInch,0,10000);
}

// what happens when the parent reaches the right side, similar to pLeftSide
void pRightSide(){
  chick.registerEndCB(centerRight);
  while(digitalRead(18) == LOW);
  delay(endDelay);
  if(digitalRead(12) == LOW) goLeft();
  else chick.runByCounts((totalTravel/2  + chickCenter)*stepsPerInch,1,6400);
}

// what happens when the chick reaches the left side
void leftSide(){
  // tell the actuator what the next step is.
  parent.registerEndCB(pLeftSide);

  // send the parent to the left.
  parent.run(0,6400);
}

// what happens when the chick reaches the right side
void rightSide(){
  parent.registerEndCB(pRightSide);
  parent.run(1,6400);
}

// what happens when the chick returns to center from the left
void centerLeft(){
  parent.registerEndCB(wait);
  parent.runByCounts((totalTravel/2 - parentCenter)*stepsPerInch,1,6400);
}

// what happens when the chick returns to center from the right
void centerRight(){
  parent.registerEndCB(wait);
  parent.runByCounts((totalTravel/2 + parentCenter) *stepsPerInch,0,6400);
}

// sends the chick to the right.
void goRight(){
  chick.registerEndCB(rightSide);
  
  // let the program know that we aren't at home.
  home = false;
  chick.run(0,6400);
}

//sends the chick to the left.
void goLeft(){
  chick.registerEndCB(leftSide);
  
  // let the program know that we aren't at home.
  home = false;
  chick.run(1,6400);
}

// what happens when the parent returns to center.
void wait(){
  // set the home flag, to let the program know we're home.
  home = true;
}

void setup() {
  chick.runTimeout = 10000;
  parent.runTimeout = 10000;
  
  parent.setSpeedProfile(parentSteps,true);

  chick.setSpeedProfile(chickSteps,true);

  //setup the 
  pinMode(leftVisSwitch,INPUT_PULLUP);
  pinMode(rightVisSwitch,INPUT_PULLUP);
  
  delay(5000);
  goLeft();
}

void loop() {
  chick.idle();
  parent.idle();

  //if we are at the home position (center), check the switches and move accordingly.
  if(home){
    if(digitalRead(leftVisSwitch) == LOW) goLeft();
    else if(digitalRead(rightVisSwitch) == LOW) goRight();
  }
}
