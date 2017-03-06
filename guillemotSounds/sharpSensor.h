#include "arduino.h"

// class for interpreting Sharp Distance sensors
class Sensor {
public:
  int pin;

  // variables for doing a rolling average.
  int numSamps;
  int * samps;
  int pntr;
  unsigned long tot;
  int ave;
  
  // vars for handling the reads
  long readTimer = 0;
  long maxTimer = 0;
  bool newRead = false;
  bool readPrep =false;
  int localMax = 0;
  int readTime = 0;

  bool cbDone = false;

  int minRead = 0;

  void (*triggerCB)(int) = NULL;

  // constructor. arguments are the analog input pin, the minimum
  // read value (between 0-1023) and the time to scan for a max value,
  // in milliseconds.
  Sensor(int p, int minrd = 150, int rtm=200){
    pin=p;
    minRead = minrd;
    readTime = rtm;

    // init the rolling average.
    pntr=tot=ave=0;
    readTimer = maxTimer = newRead = readPrep = localMax = 0;
    numSamps = 20;
    samps = new int[numSamps];
    for(int i=0; i<numSamps; i++){
      samps[i]=0;
    }
  }

  // allows the trigger CB to be called again,
  // even if the input is still above the min read threshold
  void reenable(){
    cbDone = false;
  }

  // returns true if the current value is greater than the min read
  // threshold.
  bool active(){
    return (localMax > minRead);
  }
  
  void idle(){
    // take a read, and do the rolling average.
    int nextRead = analogRead(pin);
    tot-=samps[pntr];
    samps[pntr]=nextRead;
    tot+=nextRead;
    ave = tot/numSamps;
    pntr=(pntr + 1)%numSamps;

    // if our rolling average is above the threshold, and we aren't already
    // looking for a maximum
    if(ave > minRead && !cbDone && maxTimer < millis() && !newRead && !readPrep){
      //start hunting for a maximum over readTime seconds.
      readPrep = true;
      maxTimer = millis() + readTime;
    } else if(ave<minRead){
      //otherwise, if the average is less than our thresh,
      // clear the callback done flag, and clear the local max.
      cbDone = false;
      localMax = 0;
    }

    // while we're hunting for the max within our timeframe
    if(maxTimer>millis()){
      // if the new ave read is greater than the last value, store it
      localMax = max(ave,localMax);
    } else if(localMax > minRead &&!newRead && readPrep){
      // if we're no longer hunting for a max, set the flag
      // indicating a new read is ready.
      newRead = true;
    }

    // if a new read is ready, and we're no longer hunting for a max,
    // and we haven't yet done the callback
    if(newRead && maxTimer < millis() && !cbDone){
      // do the callback if the max is above our min thresh
      if(triggerCB && localMax > minRead) triggerCB(localMax);

      //indicate that the callback was done
      cbDone = true;

      // and clear the new read flag.
      newRead = false;
      readPrep = false;
    }

  }
};

