#include "arduino.h"

// class to control linear actuators from robotZone
class actuator {
public:
  // variable declarations
  int pwmPin;
  int dirPin;
  int potPin;
  int dir;
  int speed;
  int target;
  signed int diff;
  signed int init;

  bool invert = false;

  // variables for averaging.
  int numSamps;
  int * samps;
  int pntr;
  unsigned long tot;
  int ave;

  bool running;

  void (*endCB)();

  // constructor funciton. Takes the direction control pin, speed
  // control pin, and analog input pin as arguments. 
  actuator(int d, int pwm, int pot, bool inv = false){
    pwmPin = pwm;
    potPin = pot;
    dirPin = d;
    speed = 0;
    dir = 0;
    endCB = 0;
    running = false;
    target = 0;
    diff = 0;
    init = 0;

    invert = inv;

    // init the averaging variables.
    pntr=tot=ave=0;
    numSamps = 20;
    samps = new int[numSamps];
    for(int i=0; i<numSamps; i++){
      samps[i]=analogRead(potPin);
      tot+=samps[i];
    }

    // init the IOs
    pinMode(pwmPin,OUTPUT);
    pinMode(dirPin,OUTPUT);
    digitalWrite(dirPin,HIGH);
    digitalWrite(pwmPin,LOW);
  }

  //////////////////////////////////////////
  // These functions aren't used in practice,
  // but are pretty much self-explanatory

  // return the current direction
  int direction(){
    return dir;
  }

  // set the speed of the movement
  void setSpeed(int sp){
    if(sp > 255) sp = 255;
    speed = sp;
    analogWrite(pwmPin,speed);
  }

  //set the direction of movement.
  void setDirection(int d){
    dir = d;
    digitalWrite(dirPin,dir);
  }

  //set the target
  void setTarget(int targ){
    target = targ;
  }
  void start(){
    analogWrite(pwmPin,speed);
  }
  void stop(){
    analogWrite(pwmPin,0);
  }

  ////////////////////////////////////////////
  // These functions are primarily used for control.

  // set the callback function for what to do once we reach target.
  void registerEndCB(void (*CB)()){
    endCB = CB;
  }

  // returns a 0 to 1 value of how complete the movement is.
  float percentDone(){
    return 1-(float(target - ave)/float(target - init));
  }

  // tell the actuator to move to a target position.
  // optionally pass a new speed for the motor.
  void run(int targ, int spd = 255){
    target = targ;
    speed = spd;
    // store the initial value for percentage calculation.
    init = ave;
    // calc the difference between where we are and the target
    diff = targ - ave;
    running = true;
    //set the direction based on the sign of the difference.
    dir = (diff<=0);
    if(invert) dir = !dir;

    //run the motor.
    digitalWrite(dirPin,dir);
    analogWrite(pwmPin,speed);
  }
  void idle(){
    // grab a new analog value and average over num samps.
    int newRead = analogRead(potPin);
    tot-=samps[pntr];
    samps[pntr]=newRead;
    tot+=newRead;
    ave = tot/numSamps;
    pntr=(pntr + 1)%numSamps;

    // find the difference between the current pos and target.
    diff = target - ave;

    // if the absolute val of the difference is less than
    // the threshold (25 as default), stop and do callback.
    if(abs(diff)<25 && running){
      running = false;
      stop();
      if(endCB) endCB();
    }
  }
};
