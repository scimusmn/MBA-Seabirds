#include "arduino.h"

class actuator {
public:
  int pwmPin;
  int dirPin;
  int potPin;
  int dir;
  int speed;
  int target;
  signed int diff;
  signed int init;

  int numSamps;
  int * samps;
  int pntr;
  unsigned long tot;
  int ave;

  bool running;

  void (*endCB)();

  actuator(int d, int pwm, int pot){
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

    pntr=tot=ave=0;
    numSamps = 20;
    samps = new int[numSamps];
    for(int i=0; i<numSamps; i++){
      samps[i]=analogRead(potPin);
      tot+=samps[i];
    }

    pinMode(pwmPin,OUTPUT);
    pinMode(dirPin,OUTPUT);
    digitalWrite(dirPin,HIGH);
    digitalWrite(pwmPin,LOW);
  }

  int direction(){
    return dir;
  }

  void setSpeed(int sp){
    speed = sp;
    analogWrite(pwmPin,speed);
  }
  void setDirection(int d){
    dir = d;
    digitalWrite(dirPin,dir);
  }
  void setTarget(int targ){
    target = targ;
  }
  void start(){
    analogWrite(pwmPin,speed);
  }
  void stop(){
    analogWrite(pwmPin,0);
  }
  void registerEndCB(void (*CB)()){
    endCB = CB;
  }
  float percentDone(){
    return 1-float(target - ave)/float(target - init);
  }
  void run(int targ, int spd = 255){
    target = targ;
    speed = spd;
    init = ave;
    diff = targ - ave;
    running = true;
    dir = (diff>=0);
    digitalWrite(dirPin,dir);
    analogWrite(pwmPin,speed);
  }
  void idle(){
    int newRead = analogRead(potPin);
    tot-=samps[pntr];
    samps[pntr]=newRead;
    tot+=newRead;
    ave = tot/numSamps;
    pntr=(pntr + 1)%numSamps;

    diff = target - ave;

    if(abs(diff)<25 && running){
      running = false;
      stop();
      if(endCB) endCB();
    }
  }
};
