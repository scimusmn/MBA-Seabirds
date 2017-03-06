#include "arduino.h"

class Sensor {
public:
  int pin;

  int numSamps;
  int * samps;
  int pntr;
  unsigned long tot;
  int ave;

  long readTimer = 0;
  long maxTimer = 0;
  bool newRead = false;
  bool readPrep =false;
  int localMax = 0;
  int readTime = 0;

  bool stillHigh = false;

  int minRead = 0;

  void (*triggerCB)(int) = NULL;

  Sensor(int p, int minrd = 150, int rtm=200){
    pin=p;
    minRead = minrd;
    readTime = rtm;
    
    pntr=tot=ave=0;
    readTimer = maxTimer = newRead = readPrep = localMax = 0;
    numSamps = 20;
    samps = new int[numSamps];
    for(int i=0; i<numSamps; i++){
      samps[i]=0;
    }
  }

  void reset(){
    stillHigh = false;
  }

  bool active(){
    return (localMax > minRead);
  }
  
  void idle(){
    int nextRead = analogRead(pin);
    tot-=samps[pntr];
    samps[pntr]=nextRead;
    tot+=nextRead;
    ave = tot/numSamps;
    pntr=(pntr + 1)%numSamps;

    if(maxTimer>millis()){
      localMax = max(ave,localMax);
    } else if(localMax > minRead &&!newRead && readPrep){
      newRead = true;
    }
    
    if(newRead && maxTimer < millis() && !stillHigh){
      if(triggerCB && localMax > minRead) triggerCB(localMax);
      stillHigh = true;
      
      newRead = false;
      readPrep = false;
    }
    
    if(ave > minRead && !stillHigh && maxTimer < millis() && !newRead && !readPrep){
      readPrep = true;
      maxTimer = millis() + readTime;
    } else if(ave<minRead){
      stillHigh = false;
      localMax = 0;
    }
  }
};

