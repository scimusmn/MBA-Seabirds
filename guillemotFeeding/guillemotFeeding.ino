#include "linearTrackActuators.h"

int homeSensor = 16;
int outSensor = 15;

int pulseOut = 3;
int dirOut = 5;
int enbOut = 6;

int visButton = 14;

actuator bird(3,5,6,16,15);

bool home = true;

void sitAtHome(){
  home = true;
}

void outAndHold(){
  while(digitalRead(visButton) == LOW);
  delay(1000);
  bird.registerEndCB(sitAtHome);
  bird.run(1);
}

void goOut(){
  home = false;
  bird.registerEndCB(outAndHold);
  bird.run(0);
}

signed int birdProfile (float osc){
  return 2400 - cos(osc)*3600;
}

void setup() {
  bird.setSpeedProfile(birdProfile,true);
  bird.incrementInterval = 2;
  
  pinMode(visButton,INPUT_PULLUP);

  delay(5000);

  outAndHold();
}

void loop() {
  bird.idle();
  if(digitalRead(14) == LOW && home) goOut();
}
