#include "robotZoneActuator.h"
#include "dcMotor.h"
#include "rotaryControl.h"

///////////////////////////////////////////////////
//                  Declarations                 //
///////////////////////////////////////////////////

// declare the rotary control object. Arguments are as follows

//    rotaryControl( pin number for input ,
//                   number of counts before triggering (default is 30),
//                   timeout before resetting counts (default is 1000 ms)){

rotaryControl crank(14,12,1000);

// declare teh linear actuator object. Arguments are as follows
//    actuator( direction pin,
//              speed pin,
//              analog input pin,
//              boolean value of whether or not to invert direction.

actuator bird(7,6,1,false);

dcMotor egg(5,3,17,16);

///////////////////////////////////////////////////
//              Configurable options             //
///////////////////////////////////////////////////

int eggOutTime = 2000; //in milliseconds


///////////////////////////////////////////////////
// You should not need to edit below this point  //
///////////////////////////////////////////////////

bool home = true;

// tells the bird to go up, and sets a call back for 
// what to do once it gets there.
void birdUp(){
  Serial.println("Bird goes up!");
  home = false;
  bird.endCB = eggOut;
  bird.run(600);
}

// tells the egg to roll out, and sets a call back for 
// what to do once it gets there.
void eggOut(){
  Serial.println("Egg goes out!");
  egg.endCB = eggBack;
  egg.run(255);
}

// tells the egg to roll back, and sets a call back for 
// what to do once it gets there.
void eggBack(){
  Serial.println("Egg goes back!");
  delay(eggOutTime);
  egg.endCB = birdDown;
  egg.run(-255);
}

void birdDown(){
  Serial.println("Bird goes down!");
  bird.endCB = wait;
  bird.run(50);
}

void wait(){
  home = true;
}

void setup() {
  // set the trigger callback for the crank
  crank.triggerCB = birdUp;

  // delay five seconds to give ourselves time to upload
  delay(5000);

  Serial.begin(9600);
}

void loop() {
  bird.idle();
  egg.idle();

  if(home) crank.idle();

}
