#include "arduino.h"

// class to control DC motors attached to a FET on a PWM output.
class dcMotor {
public:
  int pwmPin;
  int dirPin;
  int dir;

  //pins for sensors
  int cwPin;
  int ccwPin;
  int speed;

  unsigned long runTimer = 0;
  int runTimeout = 0;

  // callback for end of movement.
  void (*endCB)();

  bool running = true;

  dcMotor(int d, int pwm, int cw, int ccw){
    pwmPin = pwm;
    dirPin = d;
    cwPin = cw;
    ccwPin = ccw;
    speed = 0;
    dir = 0;
    endCB = NULL;

    // init IOs
    pinMode(pwmPin,OUTPUT);
    pinMode(dirPin,OUTPUT);
    digitalWrite(dirPin,LOW);
    digitalWrite(pwmPin,LOW);

    pinMode(ccwPin,INPUT_PULLUP);
    pinMode(cwPin,INPUT_PULLUP);
  }

  // stop function
  void stop(){
    running = false;
    analogWrite(pwmPin,0);
  }

  // run function. takes one input, and determines 
  // direction from the sign. 
  void run(int spd){
    running = true;
    speed = abs(spd);
    dir = (spd>=0);
    digitalWrite(dirPin,dir);
    analogWrite(pwmPin,speed);
    runTimer = millis() + runTimeout;
  }

  // run without looking for the stops.
  void runContinuous(int spd){
    speed = abs(spd);
    dir = (spd>=0);
    digitalWrite(dirPin,dir);
    analogWrite(pwmPin,speed);
  }

  bool negativeSensor(){
    return digitalRead(cwPin);
  }

  bool positiveSensor(){
    return digitalRead(ccwPin);
  }

  void idle(){
    // if we're running and looking for the stops,
    if(running){
      if(digitalRead(cwPin) == LOW && !dir){
        //if we're running clockwise and hit the cw stop,
        // stop and do callback.
        stop();
        if(endCB) endCB();
      } else if(digitalRead(ccwPin) == LOW && dir){
        //if we're running counterclockwise and hit the ccw stop,
        // stop and do callback.
        stop();
        if(endCB) endCB();
      }

      if(runTimer < millis()){
        stop();
        if(endCB) endCB();
      }
    }
  }
};
