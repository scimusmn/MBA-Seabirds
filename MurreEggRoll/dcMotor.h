#include "arduino.h"

class dcMotor {
public:
  int pwmPin;
  int dirPin;
  int dir;
  int cwPin;
  int ccwPin;
  int speed;

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

    pinMode(pwmPin,OUTPUT);
    pinMode(dirPin,OUTPUT);
    digitalWrite(dirPin,LOW);
    digitalWrite(pwmPin,LOW);

    pinMode(ccwPin,INPUT_PULLUP);
    pinMode(cwPin,INPUT_PULLUP);
  }
  
  void stop(){
    running = false;
    analogWrite(pwmPin,0);
  }
  
  void run(int spd){
    running = true;
    speed = abs(spd);
    dir = (spd>=0);
    digitalWrite(dirPin,dir);
    analogWrite(pwmPin,speed);
  }

  void runContinuous(int spd){
    speed = abs(spd);
    dir = (spd>=0);
    digitalWrite(dirPin,dir);
    analogWrite(pwmPin,speed);
  }
  
  void idle(){
    if(running){
      if(digitalRead(cwPin) == LOW && !dir){
        stop();
        if(endCB) endCB();
      } else if(digitalRead(ccwPin) == LOW && dir){
        stop();
        if(endCB) endCB();
      }
    }
  }
};
