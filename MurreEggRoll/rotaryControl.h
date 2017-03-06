#include "arduino.h"


class rotaryControl {
public:
  int hallInput;
  int prevState;
  unsigned long timer;
  int timeout;
  int counts;
  int maxCounts;

  void (*triggerCB)();
  void (*startCB)();
  void (*resetCB)();

  // declaration function. Takes the input pin, number of
  // counts to trigger, and the reset time as arguments. 
  rotaryControl(int pin, int maxCnt = 30, int TO = 1000){
    hallInput = pin;
    prevState = 1;
    timer = 0;
    timeout = TO;
    counts = 0;
    maxCounts = maxCnt;

    triggerCB = startCB = resetCB = NULL;

    pinMode(hallInput,INPUT_PULLUP);
  }

  void idle(){
    if(digitalRead(hallInput) != prevState){
      //reset the timeout counter
      timer = millis() + timeout;

      //store the current state
      prevState = !prevState;

      if(prevState){
        //increment the counter, up until jumpCount
        if(counts < maxCounts) counts++;
  
        //print the number of counts seen
        //Serial.println(counts,DEC);
  
        // if we're home, and we've seen three counts, do start callback.
        if(counts == 1 && startCB) startCB();

        //if we've seen maxCounts,
        if(counts == maxCounts){
  
          //reset the counter
          counts = 0;

          // and do the trigger callback
          if(triggerCB) triggerCB();
        }
      }
    }

    if(timer < millis() && counts){
      //reset the hall counter
      counts = 0;
  
      if(resetCB) resetCB();
    }
  }

};

