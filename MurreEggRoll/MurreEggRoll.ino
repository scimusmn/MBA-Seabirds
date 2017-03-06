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

actuator bird(7,6,1);

dcMotor egg(5,3,17,16);

///////////////////////////////////////////////////
//              Configurable options             //
///////////////////////////////////////////////////

int eggOutTime = 2000; //in milliseconds


///////////////////////////////////////////////////
// You should not need to edit below this point  //
///////////////////////////////////////////////////

bool home = true;

/*class hBridge {
public:
  int pwmPin;
  int dirPin;
  int dir;
  int speed;

  hBridge(int d, int pwm){
    pwmPin = pwm;
    dirPin = d;
    speed = 0;
    dir = 0;

    pinMode(pwmPin,OUTPUT);
    pinMode(dirPin,OUTPUT);
    digitalWrite(dirPin,LOW);
    digitalWrite(pwmPin,LOW);
  }

  int direction(){
    return dir;
  }

  void setSpeed(int sp){
    speed = sp;
  }
  void setDirection(int d){
    dir = d;
    digitalWrite(dirPin,dir);
  }
  void start(){
    analogWrite(pwmPin,speed);
  }
  void stop(){
    analogWrite(pwmPin,0);
  }
  void run(int spd){
    speed = abs(spd);
    dir = (spd>=0);
    digitalWrite(dirPin,dir);
    analogWrite(pwmPin,speed);
  }
};*/

void birdUp(){
  Serial.println("Bird goes up!");
  home = false;
  bird.endCB = eggOut;
  bird.run(600);
}

void eggOut(){
  Serial.println("Egg goes out!");
  egg.endCB = eggBack;
  egg.run(255);
}

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
  //pinMode(homeSensor,INPUT_PULLUP);
  //pinMode(outSensor,INPUT_PULLUP);
  crank.triggerCB = birdUp;

  delay(5000);

  Serial.begin(9600);
}

void loop() {
  bird.idle();
  egg.idle();

  if(home) crank.idle();

  /*bird.run(-255);
  while(analogRead(laPot)<600);
  bird.stop();
  egg.run(255);
  while (digitalRead(homeSensor) != LOW);
  egg.stop();
  delay(500);
  egg.run(-255);
  while (digitalRead(outSensor) != LOW);
  egg.stop();
  bird.run(255);
  while(analogRead(laPot)>=100);
  bird.stop();

  delay(2000);*/
}
